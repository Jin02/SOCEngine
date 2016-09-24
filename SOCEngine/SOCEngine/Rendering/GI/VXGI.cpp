#include "VXGI.h"
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

VXGI::VXGI()
	:
	_staticInfoCB(nullptr), _dynamicInfoCB(nullptr),
	_voxelization(nullptr), _mipmap(nullptr),
	_injectPointLight(nullptr), _injectSpotLight(nullptr),
	_voxelConeTracing(nullptr), _injectionColorMap(nullptr), _clearVoxelMapCS(nullptr),
	_debugVoxelViewer(nullptr), _startCenterWorldPos(0, 0, 0)
{
}

VXGI::~VXGI()
{
	SAFE_DELETE(_staticInfoCB);
	SAFE_DELETE(_dynamicInfoCB);

	SAFE_DELETE(_voxelization);

	SAFE_DELETE(_injectPointLight);
	SAFE_DELETE(_injectSpotLight);

	SAFE_DELETE(_mipmap);
	SAFE_DELETE(_voxelConeTracing);
	SAFE_DELETE(_injectionColorMap);
	SAFE_DELETE(_clearVoxelMapCS);
	SAFE_DELETE(_debugVoxelViewer);
}

void VXGI::InitializeClearVoxelMap(uint dimension)
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
		threadGroup.y = ComputeThreadGroupSideLength(dimension);
		threadGroup.z = ComputeThreadGroupSideLength(dimension);
	}

	_clearVoxelMapCS = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_clearVoxelMapCS->Initialize(), "Error, Can't Init ClearVoxelMapCS");

	std::vector<ShaderForm::InputUnorderedAccessView> uavs;
	uavs.push_back(ShaderForm::InputUnorderedAccessView(0, _injectionColorMap->GetUnorderedAccessView()));

	_clearVoxelMapCS->SetUAVs(uavs);
}

void VXGI::ClearInjectColorVoxelMap(const Device::DirectX* dx)
{
	_clearVoxelMapCS->Dispatch(dx->GetContext());
}

void VXGI::Initialize(const Device::DirectX* dx, uint dimension, float minWorldSize)
{
	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

	const uint mipmapGenOffset		= 2;
	const uint mipmapLevels			= max((uint)Log2((float)dimension) - mipmapGenOffset + 1, 1);

	// Setting Infos
	{
		_staticInfoCB = new ConstBuffer;
		_staticInfoCB->Initialize(sizeof(VXGIStaticInfo));

		_staticInfo.dimension				= dimension;
		_staticInfo.maxMipLevel				= mipmapLevels;
		_staticInfo.voxelSize				= minWorldSize / float(dimension);
		_staticInfo.diffuseHalfConeMaxAngle	= Math::Common::Deg2Rad(60.0f);

		_staticInfoCB->UpdateSubResource(dx->GetContext(), &_staticInfo);

		_dynamicInfoCB = new ConstBuffer;
		_dynamicInfoCB->Initialize(sizeof(VXGIDynamicInfo));
	}

	// Init Voxelization
	{
		_voxelization = new Voxelization;
		_voxelization->Initialize(dimension);
	}

	// Injection
	{
		_injectionColorMap = new VoxelMap;
		_injectionColorMap->Initialize(dimension, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels, true);

		_injectPointLight		= new InjectRadianceFromPointLIght;
		_injectPointLight->Initialize();

		_injectSpotLight		= new InjectRadianceFromSpotLIght;
		_injectSpotLight->Initialize();
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

	InitializeClearVoxelMap(dimension);

	_debugVoxelViewer = new Debug::VoxelViewer;
	_debugVoxelViewer->Initialize(dimension, 0);
}

void VXGI::Run(const Device::DirectX* dx, const Camera::MeshCamera* camera, const Core::Scene* scene)
{
	ASSERT_COND_MSG(camera, "Error, camera is null");

	const LightManager* lightMgr				= scene->GetLightManager();
	
	VXGIDynamicInfo dynamicInfo;
	dynamicInfo.packedNumfOfLights	= lightMgr->GetPackedLightCount();
	dynamicInfo.startCenterWorldPos	= _startCenterWorldPos;

	UpdateGIDynamicInfo(dx, dynamicInfo);

	ClearInjectColorVoxelMap(dx);

	const RenderManager* renderManager = scene->GetRenderManager();
	
	// 1. Voxelization Pass
	{
		// Clear Voxel Map and voxelize
		_voxelization->Voxelize(dx, _dynamicInfo.startCenterWorldPos, scene,
								_staticInfo.dimension, _staticInfo.voxelSize,
								_injectionColorMap, _staticInfoCB, _dynamicInfoCB);
	}

	MaterialManager* materialMgr = scene->GetMaterialManager();
	_debugVoxelViewer->GenerateVoxelViewer(dx, _voxelization->GetVoxelAlbedoRawBuffer()->GetUnorderedAccessView()->GetView(), 0, false, _staticInfo.voxelSize * float(_staticInfo.dimension), materialMgr);
	return;

	// 2. Injection Pass
	{
		const ShadowRenderer* shadowRenderer		= scene->GetShadowManager();

		InjectRadiance::DispatchParam param;
		{
			param.dimension = _staticInfo.dimension;
			param.global.vxgiDynamicInfo			= _dynamicInfoCB;
			param.global.vxgiStaticInfo				= _staticInfoCB;
			param.OutAnisotropicVoxelColorMap		= _injectionColorMap->GetSourceMapUAV();
			param.shadowGlobalInfo					= shadowRenderer->GetShadowGlobalParamConstBuffer();
			param.voxelization.AlbedoRawBuffer		= _voxelization->GetVoxelAlbedoRawBuffer();
			param.voxelization.NormalRawBuffer		= _voxelization->GetVoxelNormalRawBuffer();
			param.voxelization.EmissionRawBuffer	= _voxelization->GetVoxelEmissionRawBuffer();
			param.voxelization.InfoCB				= _voxelization->GetInfoCB();
		}

		if(lightMgr->GetPointLightCount() > 0)
			_injectPointLight->Inject(dx, lightMgr, shadowRenderer, param);

		if(lightMgr->GetSpotLightCount() > 0)
			_injectSpotLight->Inject(dx, lightMgr, shadowRenderer, param);
	}
	//_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionColorMap->GetSourceMapUAV()->GetView(), 0, false, _globalInfo.initWorldSize, materialMgr);

	// 3. Mipmap Pass
	_mipmap->Mipmapping(dx, _injectionColorMap);
	//_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionColorMap->GetMipmapUAV(0)->GetView(), 0, false, _globalInfo.initWorldSize, materialMgr);

	// 4. Voxel Cone Tracing Pass
	{
		_voxelConeTracing->Run(dx, _injectionColorMap, camera, _staticInfoCB, _dynamicInfoCB);
	}
}

void VXGI::UpdateGIDynamicInfo(const Device::DirectX* dx, const VXGIDynamicInfo& dynamicInfo)
{
	bool isChanged = memcmp(&_dynamicInfo, &dynamicInfo, sizeof(VXGIDynamicInfo)) != 0;
	if(isChanged)
	{
		_dynamicInfoCB->UpdateSubResource(dx->GetContext(), &dynamicInfo);
		_dynamicInfo = dynamicInfo;
	}
}


void VXGI::Destroy()
{
	_staticInfoCB->Destroy();
	_dynamicInfoCB->Destroy();

	_voxelization->Destroy();
	_injectPointLight->Destroy();
	_injectSpotLight->Destroy();
	_mipmap->Destroy();
	_voxelConeTracing->Destroy();
	_injectionColorMap->Destroy();
}