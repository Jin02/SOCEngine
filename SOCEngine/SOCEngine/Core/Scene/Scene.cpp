#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"
#include "FontLoader.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;

Scene::Scene(void) : 
	_cameraMgr(nullptr), _uiManager(nullptr),
	_renderMgr(nullptr)
{
	_state = State::Init;
}

Scene::~Scene(void)
{
	Destroy();
}

void Scene::Initialize()
{
	_cameraMgr		= new Manager::CameraManager;

	_renderMgr		= new Manager::RenderManager;
	_renderMgr->TestInit();

	_uiManager		= new UI::Manager::UIManager;

	_dx				= Device::Director::GetInstance()->GetDirectX();

	_lightManager	= new Manager::LightManager;

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		(*iter)->Update(dt);

	_lightManager->UpdateBuffer();
}

void Scene::RenderPreview()
{
	OnRenderPreview();

	const std::vector<Camera::CameraForm*>& cameras = _cameraMgr->GetVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		(*iter)->RenderPreviewWithUpdateTransformCB(_rootObjects.GetVector());
}

void Scene::Render()
{
#ifndef DEPRECATED_MESH_RENDERER
	auto CamIteration = [&](Camera::CameraForm* cam)
	{
		cam->Render();
	};
	_cameraMgr->IterateContent(CamIteration);

#endif
	
	_dx->GetSwapChain()->Present(0, 0);
	OnRenderPost();
}

void Scene::Destroy()
{
	UI::FontLoader::GetInstance()->Destroy();

	SAFE_DELETE(_cameraMgr);
	SAFE_DELETE(_renderMgr);
	SAFE_DELETE(_uiManager);
	SAFE_DELETE(_lightManager);

	OnDestroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}

void Scene::AddObject(Core::Object* object)
{
	_rootObjects.Add(object->GetName(), object);
}

void Scene::Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(_state == State::Loop)
		OnInput(mouse, keyboard);
}