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
	_voxelConeTracing(nullptr), _injectionSourceMap(nullptr), _mipmappedInjectionMap(nullptr), _clearVoxelMapCS(nullptr),
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

	SAFE_DELETE(_injectionSourceMap);
	SAFE_DELETE(_mipmappedInjectionMap);

	SAFE_DELETE(_clearVoxelMapCS);
	SAFE_DELETE(_debugVoxelViewer);
}

void VXGI::InitializeClearVoxelMap(uint dimension)
{
	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "ClearVoxelMap");

		ASSERT_MSG_IF(filePath.empty() == false, "Error, File path is empty");
	}

	ShaderManager* shaderMgr = ResourceManager::SharedInstance()->GetShaderManager();
	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);

	_clearVoxelMapCS = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	ASSERT_MSG_IF(_clearVoxelMapCS->Initialize(), "Error, Can't Init ClearVoxelMapCS");
}

void VXGI::ClearInjectColorVoxelMap(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();
	auto Clear = [](ID3D11DeviceContext* context, ComputeShader* clearShader, const View::UnorderedAccessView* uav, uint sideLength, bool isAnisotropic)
	{
		auto ComputeThreadGroupSideLength = [](uint sideLength)
		{
			return (uint)((float)(sideLength + 8 - 1) / 8.0f);
		};

		ComputeShader::ThreadGroup threadGroup;
		{
			threadGroup.x = ComputeThreadGroupSideLength(sideLength * (isAnisotropic ? (uint)VoxelMap::Direction::Num : 1));
			threadGroup.y = ComputeThreadGroupSideLength(sideLength);
			threadGroup.z = ComputeThreadGroupSideLength(sideLength);
		}
		clearShader->SetThreadGroupInfo(threadGroup);

		ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(0), uav);
		clearShader->Dispatch(context);
		ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(0), nullptr);
	};

	Clear(context, _clearVoxelMapCS, _injectionSourceMap->GetSourceMapUAV(), _staticInfo.dimension, false);
	Clear(context, _clearVoxelMapCS, _mipmappedInjectionMap->GetSourceMapUAV(), _staticInfo.dimension / 2, true);
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
		_staticInfo.diffuseSamplingCount		= 128;
		_staticInfo.specularSamplingCount		= 256;

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
		_injectionSourceMap = new VoxelMap;
		_injectionSourceMap->Initialize(dimension, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, 1, false);

		_mipmappedInjectionMap = new VoxelMap;
		_mipmappedInjectionMap->Initialize(dimension / 2, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels - 1, true);

		_injectPointLight		= new InjectRadianceFromPointLIght;
		_injectPointLight->Initialize();

		_injectSpotLight		= new InjectRadianceFromSpotLIght;
		_injectSpotLight->Initialize();
	}

	// Mipmap
	{
		_mipmap = new MipmapVoxelMap;
		_mipmap->Initialize();
	}

	// Voxel Cone Tracing
	{
		_voxelConeTracing = new VoxelConeTracing;
		_voxelConeTracing->Initialize(dx);
	}

	InitializeClearVoxelMap(dimension);

//	_debugVoxelViewer = new Debug::VoxelViewer;
//	_debugVoxelViewer->Initialize(dimension, false, false);
}

void VXGI::Run(const Device::DirectX* dx, const Camera::MeshCamera* camera, const Core::Scene* scene)
{
	ASSERT_MSG_IF(camera, "Error, camera is null");

	const LightManager* lightMgr	= scene->GetLightManager();
	
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
								_injectionSourceMap, _staticInfoCB, _dynamicInfoCB);
	}

	MaterialManager* materialMgr = scene->GetMaterialManager();
//	_debugVoxelViewer->GenerateVoxelViewer(dx, _voxelization->GetVoxelNormalRawBuffer()->GetUnorderedAccessView()->GetView(), 0, false, _staticInfo.voxelSize * float(_staticInfo.dimension), materialMgr);
//	return;

	// 2. Injection Pass
	{
		const ShadowRenderer* shadowRenderer		= scene->GetShadowManager();

		InjectRadiance::DispatchParam param;
		{
			param.dimension = _staticInfo.dimension;
			param.global.vxgiDynamicInfo			= _dynamicInfoCB;
			param.global.vxgiStaticInfo				= _staticInfoCB;
			param.OutVoxelColorMap					= _injectionSourceMap->GetSourceMapUAV();
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

//	_debugVoxelViewer->GenerateVoxelViewer(dx, _injectionSourceMap->GetSourceMapUAV()->GetView(), 0, false, _staticInfo.voxelSize * float(_staticInfo.dimension), materialMgr);
//	return;

	// 3. Mipmap Pass
	_mipmap->Mipmapping(dx, _injectionSourceMap, _mipmappedInjectionMap);
//ok_debugVoxelViewer->GenerateVoxelViewer(dx, _mipmappedInjectionMap->GetSourceMapUAV()->GetView(), 0, false, _staticInfo.voxelSize * float(_staticInfo.dimension), materialMgr);
//ok_debugVoxelViewer->GenerateVoxelViewer(dx, _mipmappedInjectionMap->GetMipmapUAV(0)->GetView(), 0, false, _staticInfo.voxelSize * float(_staticInfo.dimension), materialMgr);
//	return;

	// 4. Voxel Cone Tracing Pass
	{
		_voxelConeTracing->Run(dx, _injectionSourceMap, _mipmappedInjectionMap, camera, _staticInfoCB, _dynamicInfoCB);
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

void VXGI::UpdateGIStaticDynamicInfo(const Device::DirectX* dx, const VXGIStaticInfo& info)
{
	// 자주 업데이트 될 것이 아니니, 중복 체크를 할 이유는 딱히 없다.
	_staticInfoCB->UpdateSubResource(dx->GetContext(), &info);	
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

	_injectionSourceMap->Destroy();
	_mipmappedInjectionMap->Destroy();
}
