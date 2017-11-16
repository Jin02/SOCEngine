#include "RenderingSystem.h"
#include "Engine.h"

using namespace Core;
using namespace Device;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Rendering::GI;

void RenderingSystem::InitializeRenderer(Engine& engine, const RenderSetting&& param)
{
	Object& object		= engine.GetObjectManager().Add(param.mainCamName);
	MainCamera& maincam	= engine.GetComponentSystem().SetMainCamera(object.GetObjectID());

	maincam.Initialize(engine.GetDirectX(), _shaderManager, param.renderRect);

	_shadowRenderer.Initialize(engine.GetDirectX(), param.shadowMapResolution, param.shadowMapResolution, param.shadowMapResolution);
	_mainRenderer.Initialize(engine.GetDirectX(), _shaderManager, maincam,
		GlobalIllumination::InitParam{
		VXGIStaticInfo(param.voxelizeDimension, param.voxelizeSize)
	});	
}

void RenderingSystem::Initialize(Engine& engine)
{
	_materialManager.Initialize(engine.GetDirectX());
	_postProcessing.Initialize(engine.GetDirectX(), _shaderManager, engine.GetComponentSystem().GetMainCamera());

	_lightManager.Initialize(engine.GetDirectX());
	_shadowManager.Initialize(engine.GetDirectX());
}

void RenderingSystem::Update(Engine& engine, float dt)
{
	_postProcessing.SetElapsedTime(dt);
}

void RenderingSystem::Render(Engine& engine)
{
	_materialManager.UpdateConstBuffer(engine.GetDirectX());
	_postProcessing.UpdateCB(engine.GetDirectX());
}

void RenderingSystem::Destroy(Engine& engine)
{

}