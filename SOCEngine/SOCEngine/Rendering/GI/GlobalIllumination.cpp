#include "GlobalIllumination.h"
#include "BindIndexInfo.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"
#include "Scene.h"

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
using namespace Math;

GlobalIllumination::GlobalIllumination()
	: _giGlobalStaticInfoCB(nullptr), _giGlobalDynamicInfoCB(nullptr),
	_voxelization(nullptr), _mipmap(nullptr),
	_injectPointLight(nullptr), _injectSpotLight(nullptr),
	_voxelConeTracing(nullptr), _injectionColorMap(nullptr), _clearVoxelMapCS(nullptr),
	_debugVoxelViewer(nullptr), _isAttachCamera(true)
{
}

GlobalIllumination::~GlobalIllumination()
{
	SAFE_DELETE(_giGlobalStaticInfoCB);
	SAFE_DELETE(_giGlobalDynamicInfoCB);

	SAFE_DELETE(_voxelization);

	SAFE_DELETE(_injectPointLight);
	SAFE_DELETE(_injectSpotLight);

	SAFE_DELETE(_mipmap);
	SAFE_DELETE(_voxelConeTracing);
	SAFE_DELETE(_injectionColorMap);
	SAFE_DELETE(_clearVoxelMapCS);
	SAFE_DELETE(_debugVoxelViewer);
}

// 귀찮아서 그냥 복붙했지만, Voxelization에 있는 것과 하나로 합쳐서 정리해야함
void GlobalIllumination::InitializeClearAnisotropicVoxelMap(uint dimension, uint maxNumOfCascade)
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

		threadGroup.x = ComputeThreadGroupSideLength(dimension * (uint)VoxelMap::Direction::Num);
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
		// Static
		{
			_globalStaticInfo.maxCascadeWithVoxelDimensionPow2	= (maxNumOfCascade << 16) | (voxelDimensionPow2 & 0xffff);
			_globalStaticInfo.maxMipLevel						= (float)mipmapLevels;

			_giGlobalStaticInfoCB = new ConstBuffer;
			_giGlobalStaticInfoCB->Initialize(sizeof(GlboalStaticInfo));
			_giGlobalStaticInfoCB->UpdateSubResource(dx->GetContext(), &_globalStaticInfo);
		}

		// Dynamic
		{
			_globalDynamicInfo.initVoxelSize						= minWorldSize / (float)dimension;
			_globalDynamicInfo.startCenterWorldPos					= Vector3(0.0f, 0.0f, 0.0f);
			_giGlobalDynamicInfoCB = new ConstBuffer;
			_giGlobalDynamicInfoCB->Initialize(sizeof(GlobalDynamicInfo));
			_giGlobalDynamicInfoCB->UpdateSubResource(dx->GetContext(), &_globalDynamicInfo);
		}
	}

	// Init Voxelization
	{
		_voxelization = new Voxelization;
		_voxelization->Initialize(maxNumOfCascade, dimension, _giGlobalStaticInfoCB);
	}

	// Injection
	{
		_injectionColorMap = new VoxelMap;
		_injectionColorMap->Initialize(	dimension, maxNumOfCascade,
										DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels, true);
		InjectRadiance::InitParam initParam;
		{
			initParam.dimension			= dimension;
			initParam.giStaticInfoCB	= _giGlobalStaticInfoCB;
			initParam.giDynamicInfoCB	= _giGlobalDynamicInfoCB;
			initParam.voxelization		= _voxelization;
			initParam.outColorMap		= _injectionColorMap;
		}

		_injectPointLight		= new InjectRadianceFromPointLIght;
		_injectPointLight->Initialize(initParam);

		_injectSpotLight		= new InjectRadianceFromSpotLIght;
		_injectSpotLight->Initialize(initParam);
	}

	// Mipmap
	{
		_mipmap = new MipmapVoxelTexture;
		_mipmap->Initialize();
	}

	// Voxel Cone Tracing
	{
		_voxelConeTracing = new VoxelConeTracing;
		_voxelConeTracing->Initialize(dx);
	}

	InitializeClearAnisotropicVoxelMap(dimension, maxNumOfCascade);

	_debugVoxelViewer = new Debug::VoxelViewer;
	_debugVoxelViewer->Initialize(dimension, false);
}

void GlobalIllumination::Run(const Device::DirectX* dx, const Camera::MeshCamera* camera, const Core::Scene* scene)
{
	ASSERT_COND_MSG(camera, "Error, camera is null");
	if(_isAttachCamera)
	{
		GlobalDynamicInfo info = _globalDynamicInfo;
		info.startCenterWorldPos = camera->GetWorldPosition();
		UpdateGIDynamicInfo(dx, info);
	}

	ClearInjectColorVoxelMap(dx);

	const RenderManager* renderManager = scene->GetRenderManager();

	uint dimension		= 1 << (_globalStaticInfo.maxCascadeWithVoxelDimensionPow2 & 0xffff);
	uint maxCascade		= _globalStaticInfo.maxCascadeWithVoxelDimensionPow2 >> 16;

	float initWorldSize = _globalDynamicInfo.initVoxelSize * static_cast<float>(dimension);

	// 1. Voxelization Pass
	{
		// Clear Voxel Map and voxelize
		_voxelization->Voxelize(dx, camera, scene, maxCascade, initWorldSize, _injectionColorMap, _giGlobalStaticInfoCB, _giGlobalDynamicInfoCB, false);
	}

	MaterialManager* materialMgr = scene->GetMaterialManager();
	_debugVoxelViewer->GenerateVoxelViewer(dx, _voxelization->GetAnisotropicVoxelAlbedoMapAtlas()->GetUnorderedAccessView()->GetView(), 0, false, initWorldSize, materialMgr);
	return;

	const ShadowRenderer* shadowRenderer = scene->GetShadowManager();

	// 2. Injection Pass
	{

		if(shadowRenderer->GetPointLightCount() > 0)
			_injectPointLight->Inject(dx, shadowRenderer, _voxelization, dimension, maxCascade);

		if(shadowRenderer->GetSpotLightCount() > 0)
			_injectSpotLight->Inject(dx, shadowRenderer, _voxelization);
	}
	//_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionColorMap->GetSourceMapUAV()->GetView(), 0, false, _globalInfo.initWorldSize, materialMgr);

	// 3. Mipmap Pass
	_mipmap->Mipmapping(dx, _injectionColorMap, maxCascade);
	//_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionColorMap->GetMipmapUAV(0)->GetView(), 0, false, _globalInfo.initWorldSize, materialMgr);

	// 4. Voxel Cone Tracing Pass
	{
		_voxelConeTracing->Run(dx, _injectionColorMap, camera, _giGlobalStaticInfoCB, _giGlobalDynamicInfoCB);
	}
}

void GlobalIllumination::UpdateGIDynamicInfo(const Device::DirectX* dx, const GlobalDynamicInfo& dynamicInfo)
{
	bool isChanged = memcmp(&_globalDynamicInfo, &dynamicInfo, sizeof(GlobalDynamicInfo)) != 0;
	if(isChanged)
	{
		_giGlobalDynamicInfoCB->UpdateSubResource(dx->GetContext(), &dynamicInfo);
		_globalDynamicInfo = dynamicInfo;
	}
}

void GlobalIllumination::Destroy()
{
	_giGlobalStaticInfoCB->Destory();
	_giGlobalDynamicInfoCB->Destory();

	_voxelization->Destroy();
	_injectPointLight->Destroy();
	_injectSpotLight->Destroy();
	_mipmap->Destroy();
	_voxelConeTracing->Destroy();
	_injectionColorMap->Destory();
}
