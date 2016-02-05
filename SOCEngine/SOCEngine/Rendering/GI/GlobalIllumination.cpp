#include "GlobalIllumination.h"
#include "BindIndexInfo.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"

using namespace Device;
using namespace Core;
using namespace Resource;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;

GlobalIllumination::GlobalIllumination()
	: _giGlobalInfoCB(nullptr), _voxelization(nullptr), _mipmap(nullptr),
	_injectDirectionalLight(nullptr), _injectPointLight(nullptr), _injectSpotLight(nullptr),
	_voxelConeTracing(nullptr), _injectionColorMap(nullptr), _clearVoxelMapCS(nullptr),
	_debugVoxelViewer(nullptr)
{
}

GlobalIllumination::~GlobalIllumination()
{
	SAFE_DELETE(_giGlobalInfoCB);
	SAFE_DELETE(_voxelization);

	SAFE_DELETE(_injectDirectionalLight);
	SAFE_DELETE(_injectPointLight);
	SAFE_DELETE(_injectSpotLight);

	SAFE_DELETE(_mipmap);
	SAFE_DELETE(_voxelConeTracing);
	SAFE_DELETE(_injectionColorMap);
	SAFE_DELETE(_clearVoxelMapCS);
	SAFE_DELETE(_debugVoxelViewer);
}

// 귀찮아서 그냥 복붙했지만, Voxelization에 있는 것과 하나로 합쳐서 정리해야함
void GlobalIllumination::InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade)
{
	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "ClearVoxelMap");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	ShaderManager* shaderMgr = ResourceManager::SharedInstance()->GetShaderManager();
	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);

	ComputeShader::ThreadGroup threadGroup;
	{
		auto ComputeThreadGroupSideLength = [](uint sideLength)
		{
			return (uint)((float)(sideLength + 8 - 1) / 8.0f);
		};
#ifdef USE_ANISOTROPIC_VOXELIZATION
		threadGroup.x = ComputeThreadGroupSideLength(dimension * (uint)VoxelMap::Direction::Num);
#else
		threadGroup.x = ComputeThreadGroupSideLength(dimension);
#endif
		threadGroup.y = ComputeThreadGroupSideLength(dimension * maxNumOfCascade);
		threadGroup.z = ComputeThreadGroupSideLength(dimension);
	}

	_clearVoxelMapCS = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_clearVoxelMapCS->Initialize(), "Error, Can't Init ClearVoxelMapCS");

	std::vector<ShaderForm::InputUnorderedAccessView> uavs;
	uavs.push_back(ShaderForm::InputUnorderedAccessView(0, _injectionColorMap->GetUnorderedAccessView()));

	_clearVoxelMapCS->SetUAVs(uavs);
}

// 귀찮아서 그냥 복붙했지만, Voxelization에 있는 것과 하나로 합쳐서 정리해야함
void GlobalIllumination::ClearInjectColorVoxelMap(const Device::DirectX* dx)
{
	_clearVoxelMapCS->Dispatch(dx->GetContext());
}
////

void GlobalIllumination::Initialize(const Device::DirectX* dx, uint dimension, float minWorldSize)
{
	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

	const uint mipmapGenOffset		= 2;
	const uint mipmapLevels			= max((uint)Log2((float)dimension) - mipmapGenOffset + 1, 1);

	uint maxNumOfCascade			= 1;
	uint voxelDimensionPow2			= (uint)Log2((float)dimension);

	// Setting GlobalInfo
	{
		_giGlobalInfoCB = new ConstBuffer;
		_giGlobalInfoCB->Initialize(sizeof(GlobalInfo));

		_globalInfo.maxCascadeWithVoxelDimensionPow2	= (maxNumOfCascade << 16) | (voxelDimensionPow2 & 0xffff);
		_globalInfo.initVoxelSize						= minWorldSize / (float)dimension;
		_globalInfo.initWorldSize						= minWorldSize;
		_globalInfo.maxMipLevel							= (float)mipmapLevels;

		_giGlobalInfoCB->UpdateSubResource(dx->GetContext(), &_globalInfo);
	}

	// Init Voxelization
	{
		_voxelization = new Voxelization;
		_voxelization->Initialize(_globalInfo, _giGlobalInfoCB);
	}

	// Injection
	{
		_injectionColorMap = new VoxelMap;
		_injectionColorMap->Initialize(	dimension, maxNumOfCascade,
										DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels, false);

		InjectRadiance::InitParam initParam;
		{
			initParam.globalInfo		= &_globalInfo;
			initParam.giInfoConstBuffer	= _giGlobalInfoCB;
			initParam.voxelization		= _voxelization;
			initParam.outColorMap		= _injectionColorMap;
		}

		_injectDirectionalLight	= new InjectRadianceFromDirectionalLIght;
		_injectDirectionalLight->Initialize(initParam);

		_injectPointLight		= new InjectRadianceFromPointLIght;
		_injectPointLight->Initialize(initParam);

		_injectSpotLight		= new InjectRadianceFromSpotLIght;
		_injectSpotLight->Initialize(initParam);
	}

	// Mipmap
	{
		_mipmap = new MipmapVoxelTexture;
		_mipmap->Initialize(_globalInfo);
	}

	// Voxel Cone Tracing
	{
		_voxelConeTracing = new VoxelConeTracing;
		_voxelConeTracing->Initialize(dx, _giGlobalInfoCB);
	}

	InitializeClearVoxelMap(dimension, maxNumOfCascade);

	_debugVoxelViewer = new Debug::VoxelViewer;
	_debugVoxelViewer->Initialize(dimension, true);
//	_debugVoxelViewer->Initialize(dimension, false);
}

void GlobalIllumination::Run(const Device::DirectX* dx, const Camera::MeshCamera* camera,
							 const Manager::RenderManager* renderManager,
							 const Shadow::ShadowRenderer* shadowRenderer,
							 Manager::MaterialManager* materialMgr)
{
	ASSERT_COND_MSG(camera, "Error, camera is null");

	// Clear Voxel Normal Map
	{
		const VoxelMap* normalMap = _voxelization->GetAnisotropicVoxelNormalMapAtlas();

		std::vector<ShaderForm::InputUnorderedAccessView> originUAVs	= _clearVoxelMapCS->GetUAVs();
		ComputeShader::ThreadGroup originThreadGroup					= _clearVoxelMapCS->GetThreadGroupInfo();
	
		// set voxel normal map info
		{
			uint count = uint( (float)( (1 << (_globalInfo.maxCascadeWithVoxelDimensionPow2 & 0xffff)) + 8 - 1 ) / 8.0f );
			_clearVoxelMapCS->SetThreadGroupInfo(ComputeShader::ThreadGroup(count, count, count));
	
			std::vector<ShaderForm::InputUnorderedAccessView> uavs;
			uavs.push_back(ShaderForm::InputUnorderedAccessView(0, normalMap->GetUnorderedAccessView()));
			_clearVoxelMapCS->SetUAVs(uavs);
		}

		_clearVoxelMapCS->Dispatch(dx->GetContext());

		// recover
		_clearVoxelMapCS->SetThreadGroupInfo(originThreadGroup);
		_clearVoxelMapCS->SetUAVs(originUAVs);
	}

	// 1. Voxelization Pass
	{
		// Clear Voxel Map and voxelize
		_voxelization->Voxelize(dx, camera, renderManager, _globalInfo, false);
	}
	
	_debugVoxelViewer->GenerateVoxelViewer(dx, _voxelization->GetAnisotropicVoxelAlbedoMapAtlas(), 0, false, _globalInfo.initWorldSize, materialMgr);

	//ClearInjectColorVoxelMap(dx);

	//// 2. Injection Pass
	//{
	//	if(shadowRenderer->GetDirectionalLightCount() > 0)
	//		_injectDirectionalLight->Inject(dx, shadowRenderer, _voxelization);

	//	if(shadowRenderer->GetPointLightCount() > 0)
	//		_injectPointLight->Inject(dx, shadowRenderer, _voxelization);

	//	if(shadowRenderer->GetSpotLightCount() > 0)
	//		_injectSpotLight->Inject(dx, shadowRenderer, _voxelization);
	//}

	//_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionColorMap, 0, false, _globalInfo.initWorldSize, materialMgr);

	// 3. Mipmap Pass
	//_mipmap->Mipmapping(dx, _injectionColorMap, (_globalInfo.maxCascadeWithVoxelDimensionPow2 >> 16));
	//_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionColorMap, 0, false, _globalInfo.initWorldSize, materialMgr);

	// 4. Voxel Cone Tracing Pass
	{
		//_voxelConeTracing->Run(dx, _injectionColorMap, _mipmap->GetAnisotropicColorMap(), camera);
	}
}

void GlobalIllumination::Destroy()
{
	_giGlobalInfoCB->Destory();
	_voxelization->Destroy();
	_injectDirectionalLight->Destroy();
	_injectPointLight->Destroy();
	_injectSpotLight->Destroy();
	_mipmap->Destroy();
	_voxelConeTracing->Destroy();
	_injectionColorMap->Destory();
}