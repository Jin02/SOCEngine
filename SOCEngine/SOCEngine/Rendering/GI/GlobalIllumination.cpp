#include "GlobalIllumination.h"
#include "BindIndexInfo.h"
#include "Director.h"

using namespace Device;
using namespace Core;
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
	_voxelConeTracing(nullptr), _injectionColorMap(nullptr)
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
}

void GlobalIllumination::Initialize(const Device::DirectX* dx, uint dimension, float minWorldSize)
{
	// Setting GlobalInfo
	{
		_giGlobalInfoCB = new ConstBuffer;
		_giGlobalInfoCB->Initialize(sizeof(GlobalInfo));

		auto Log2 = [](float v) -> float
		{
			return log(v) / log(2.0f);
		};

		const uint mipmapGenOffset		= 4;
		const uint mipmapLevels			= max((uint)Log2((float)dimension) + 1 - mipmapGenOffset, 1);

		_globalInfo.maxNumOfCascade		= 1;
		_globalInfo.voxelDimensionPow2	= (uint)Log2((float)dimension);
		_globalInfo.initVoxelSize		= minWorldSize / (float)dimension;
		_globalInfo.initWorldSize		= minWorldSize;
		_globalInfo.maxMipLevel			= (float)mipmapLevels;

		_giGlobalInfoCB->UpdateSubResource(dx->GetContext(), &_globalInfo);
	}

	// Init Voxelization
	{
		_voxelization = new Voxelization;
		_voxelization->Initialize(_globalInfo);
	}

	// Injection
	{
		_injectionColorMap = new AnisotropicVoxelMapAtlas;
		_injectionColorMap->Initialize(	dimension, _globalInfo.maxNumOfCascade,
										DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, _globalInfo.maxMipLevel);

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
		_mipmap = new MipmapAnisotropicVoxelMapAtlas;
		_mipmap->Initialize();
	}

	// Voxel Cone Tracing
	{
		_voxelConeTracing = new VoxelConeTracing;
		_voxelConeTracing->Initialize(dx);
	}
}

void GlobalIllumination::Run(const Device::DirectX* dx, const Camera::MeshCamera* camera,
							 const Manager::RenderManager* renderManager,
							 const Shadow::ShadowRenderer* shadowRenderer)
{
	ASSERT_COND_MSG(camera, "Error, camera is null");

	// 1. Voxelization Pass
	{
		// Clear Voxel Map and voxelize
		_voxelization->Voxelize(dx, camera, renderManager, _globalInfo, false);
	}

	// 2. Injection Pass
	{
		if(shadowRenderer->GetDirectionalLightCount() > 0)
			_injectDirectionalLight->Inject(dx, shadowRenderer, _voxelization);

		if(shadowRenderer->GetPointLightCount() > 0)
			_injectPointLight->Inject(dx, shadowRenderer, _voxelization);

		if(shadowRenderer->GetSpotLightCount() > 0)
			_injectSpotLight->Inject(dx, shadowRenderer, _voxelization);
	}

	// 3. Mipmap Pass
	_mipmap->Mipmapping(dx, _injectionColorMap, _globalInfo.maxNumOfCascade);

	// 4. Voxel Cone Tracing Pass
	{
		VoxelConeTracing::DirectLightingParam param;
		{
			param.gbuffer.albedo_emission	= camera->GetGBufferAlbedoEmission();
			param.gbuffer.normal_roughness	= camera->GetGBufferNormalRoughness();
			param.gbuffer.specular_metallic = camera->GetGBufferSpecularMetallic();
			param.opaqueDepthBuffer			= camera->GetOpaqueDepthBuffer();
			param.directLightingColorMap	= camera->GetUncompressedOffScreen();
		}

		_voxelConeTracing->Run(dx, _injectionColorMap, param);
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