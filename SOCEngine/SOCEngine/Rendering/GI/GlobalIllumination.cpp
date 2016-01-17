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
	_injectDirectionalLight(nullptr), _injectPointLight(nullptr), _injectSpotLight(nullptr)
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

		const uint mipmapLevels = min((uint)Log2((float)dimension) + 1, 1);

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
		InjectRadiance::InitParam initParam;
		{
			initParam.globalInfo		= &_globalInfo;
			initParam.giInfoConstBuffer	= _giGlobalInfoCB;
			initParam.albedoMap			= _voxelization->GetAnisotropicVoxelAlbedoMapAtlas();
			initParam.normalMap			= _voxelization->GetAnisotropicVoxelNormalMapAtlas();
			initParam.emissionMap		= _voxelization->GetAnisotropicVoxelEmissionMapAtlas();
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
}

void GlobalIllumination::Run(const Device::DirectX* dx, const Camera::CameraForm*& camera, const Core::Scene* curScene)
{
	const RenderManager* renderManager		= curScene->GetRenderManager();
	const ShadowRenderer* shadowRenderer	= curScene->GetShadowManager();

	// 1. Voxelization Pass
	{
		// Clear Voxel Map and voxelize
		_voxelization->Voxelize(dx, camera, renderManager, _globalInfo, false);
	}

	// 2. Injection Pass
	{
		if(shadowRenderer->GetDirectionalLightCount() > 0)
			_injectDirectionalLight->Inject(dx, shadowRenderer, _voxelization->GetConstBuffers());

		if(shadowRenderer->GetDirectionalLightCount() > 0)
			_injectPointLight->Inject(dx, shadowRenderer, _voxelization->GetConstBuffers());

		if(shadowRenderer->GetDirectionalLightCount() > 0)
			_injectSpotLight->Inject(dx, shadowRenderer, _voxelization->GetConstBuffers());
	}

	// 3. Mipmapping Pass
	{
		_mipmap->Mipmapping(dx, _injectDirectionalLight, _globalInfo.maxNumOfCascade);
		_mipmap->Mipmapping(dx, _injectPointLight, _globalInfo.maxNumOfCascade);
		_mipmap->Mipmapping(dx, _injectSpotLight, _globalInfo.maxNumOfCascade);
	}

	// 4. Voxel Cone Tracing Pass
	{

	}
}

void GlobalIllumination::Destroy()
{
}