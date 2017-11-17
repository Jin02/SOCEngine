#include "RenderingSystem.h"
#include "Engine.h"

using namespace Core;
using namespace Device;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Rendering::GI;
using namespace Rendering::Manager;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Material;

void RenderingSystem::InitializeRenderer(Engine& engine, const RenderSetting&& param)
{
	Object& object		= engine.GetObjectManager().Add(param.mainCamName);
	MainCamera& maincam	= engine.GetComponentSystem().SetMainCamera(object.GetObjectID());

	maincam.Initialize(engine.GetDirectX(), _shaderManager, param.renderRect);
	engine.AddRootObject(object);

	_shadowRenderer.Initialize(engine.GetDirectX(), param.shadowMapResolution, param.shadowMapResolution, param.shadowMapResolution);
	_mainRenderer.Initialize(engine.GetDirectX(), _shaderManager, maincam,
		GlobalIllumination::InitParam{
		VXGIStaticInfo(param.voxelizeDimension, param.voxelizeSize)
	});	

	_defaultShaders.Initialize(engine.GetDirectX(), _shaderManager);
}

void RenderingSystem::Initialize(Engine& engine)
{
	_materialManager.Initialize(engine.GetDirectX());
	_postProcessing.Initialize(engine.GetDirectX(), _shaderManager, engine.GetComponentSystem().GetMainCamera());
}

void RenderingSystem::Update(Engine& engine, float dt)
{
	_postProcessing.SetElapsedTime(dt);
}

void RenderingSystem::Render(Engine& engine)
{
	auto& dx				= engine.GetDirectX();
	const auto& compoSys	= engine.GetComponentSystem();
	const auto& mainCamera	= compoSys.GetMainCamera();

	_materialManager.UpdateConstBuffer(dx);

	const auto& shadowMgr		= compoSys.GetManager_Direct<ShadowManager>();
	const auto& lightMgr		= compoSys.GetManager_Direct<LightManager>();
	const auto cullParam		= engine.GetCullingParam();
	const auto meshRenderParam	= GetMeshRenderParam();

	_shadowRenderer.RenderShadowMap<SpotLightShadow>(dx, shadowMgr, _materialManager, cullParam, meshRenderParam);
	_shadowRenderer.RenderShadowMap<PointLightShadow>(dx, shadowMgr, _materialManager, cullParam, meshRenderParam);
	_shadowRenderer.RenderShadowMap<DirectionalLightShadow>(dx, shadowMgr, _materialManager, cullParam, meshRenderParam);

	SkyBoxMaterial* skyboxMaterial = _materialManager.Find<SkyBoxMaterial>(mainCamera.GetSkyBoxMaterialID());
	_mainRenderer.UpdateCB(dx, mainCamera, lightMgr);
	_mainRenderer.Render(dx, MainRenderer::Param{mainCamera, meshRenderParam, _materialManager, lightMgr, shadowMgr, _shadowRenderer, std::move(cullParam), skyboxMaterial});

	_postProcessing.UpdateCB(dx);
	_postProcessing.Render(dx, _mainRenderer, mainCamera);
}

void RenderingSystem::Destroy(Engine& engine)
{

}