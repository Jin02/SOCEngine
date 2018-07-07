#include "RenderingSystem.h"
#include "Engine.h"
#include "AutoBinder.hpp"

#undef min

using namespace Core;
using namespace Device;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shader;
using namespace Rendering::Renderer;
using namespace Rendering::RenderState;
using namespace Rendering::GI;
using namespace Rendering::Manager;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Material;

void RenderingSystem::InitializeRenderer(Engine& engine, const RenderSetting&& param)
{
	Object& object		= engine.GetObjectManager().Acquire(param.mainCamName);
	MainCamera& maincam	= engine.GetComponentSystem().SetMainCamera(object.GetObjectID());

	maincam.Initialize(engine.GetDirectX(), _shaderManager, param.renderRect);
	engine.AddRootObject(object);

	_shadowRenderer.Initialize(engine.GetDirectX(), param.shadowMapResolution, param.shadowMapResolution, param.shadowMapResolution);
	_mainRenderer.Initialize(engine.GetDirectX(), _shaderManager, _bufferManager, maincam, param.giParam);	

	_defaultShaders.Initialize(engine.GetDirectX(), _shaderManager);
	_backBufferMaker.Initialize(engine.GetDirectX(), _shaderManager);
}

void RenderingSystem::Initialize(Engine& engine, bool useBloom)
{
	_materialManager.Initialize(engine.GetDirectX());
	_postProcessing.Initialize(engine.GetDirectX(), _shaderManager, engine.GetComponentSystem().GetMainCamera(), useBloom);
}

void RenderingSystem::Update(Engine& engine, float dt)
{
	_postProcessing.SetElapsedTime(dt);
}

void RenderingSystem::Render(Engine& engine, float dt)
{
	auto& dx = engine.GetDirectX();
	_materialManager.UpdateConstBuffer(dx);

	const auto& compoSys		= engine.GetComponentSystem();
	const auto& shadowMgr		= compoSys.GetManager_Direct<ShadowManager>();
	const auto cullParam		= engine.GetCullingParam();
	const auto meshRenderParam	= GetMeshRenderParam();
	_shadowRenderer.RenderShadowMap<SpotLightShadow>(dx, shadowMgr, _materialManager, cullParam, meshRenderParam);
	_shadowRenderer.RenderShadowMap<PointLightShadow>(dx, shadowMgr, _materialManager, cullParam, meshRenderParam);
	_shadowRenderer.RenderShadowMap<DirectionalLightShadow>(dx, shadowMgr, _materialManager, cullParam, meshRenderParam);

	const auto& lightMgr					= compoSys.GetManager_Direct<LightManager>();
	const auto& mainCamera					= compoSys.GetMainCamera();

	if (_useSkyScattering)
	{
		_skyScatteringRenderer.CheckRenderAbleWithUpdateCB(engine.GetDirectX(), lightMgr, mainCamera);
		
		if (_skyScatteringRenderer.GetRenderAble())
			_skyScatteringRenderer.Render(engine.GetDirectX(), mainCamera, lightMgr);
	}

	_mainRenderer.UpdateCB(dx, mainCamera, lightMgr);

	const SkyBoxMaterial* skyboxMaterial	= _materialManager.Find<SkyBoxMaterial>(mainCamera.GetSkyBoxMaterialID());
	_mainRenderer.Render(dx, MainRenderer::Param{mainCamera, meshRenderParam, _materialManager, lightMgr, ShadowSystem{shadowMgr, _shadowRenderer}, std::move(cullParam), skyboxMaterial});

	_postProcessing.SetElapsedTime(dt);
	_postProcessing.UpdateCB(dx, cullParam.objMgr, lightMgr, cullParam.transformPool, mainCamera);
	_postProcessing.Render(dx, _mainRenderer, mainCamera, lightMgr);

	AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex(0), SamplerState::Point);
	_backBufferMaker.Render(dx, dx.GetBackBufferRT(), *_mainRenderer.GetResultMap()->GetTexture2D());

	dx.GetSwapChain()->Present(0, 0);

	_shadowRenderer.ClearDirty();
}

void RenderingSystem::Destroy(Engine& engine)
{
	_postProcessing.Destroy();
	_mainRenderer.Destroy();
	_shadowRenderer.Destroy();
	_skyScatteringRenderer.Destroy();

	_materialManager.Destroy();
	_defaultShaders.Destroy();
	_bufferManager.Destroy();
	_shaderManager.Destroy();
	_tex2dManager.DeleteAll();
}

MaterialID RenderingSystem::ActivateSkyScattering(Engine& engine, uint resolution, const Object& directionalLightObject)
{
	assert(_useSkyScattering == false);
	_useSkyScattering = true;

	Size<uint> viewport = engine.GetDirectX().GetBackBufferRect().size.Cast<uint>();
	uint minSize = std::min(viewport.w, viewport.h);
	auto Log2Uint = [](uint i) -> uint
	{
		return static_cast<uint>(log(static_cast<float>(i)) / log(2.0f));
	};
	resolution = std::min(static_cast<uint>(1 << Log2Uint(minSize)), resolution);

	_skyScatteringRenderer.Initialize(engine.GetDirectX(), _bufferManager, _shaderManager, _materialManager, resolution);
	
	assert(directionalLightObject.HasComponent<DirectionalLight>());
	_skyScatteringRenderer.SetDirectionalLightID(directionalLightObject.GetObjectID());

	return _skyScatteringRenderer.GetMaterialID();
}

void RenderingSystem::DeactivateSkyScattering()
{
	assert(_useSkyScattering);
	_useSkyScattering = false;

	_skyScatteringRenderer.Destroy();
}
