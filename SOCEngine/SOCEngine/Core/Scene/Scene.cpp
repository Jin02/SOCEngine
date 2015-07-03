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
	_renderMgr(nullptr), _uiCamera(nullptr),
	_backBufferMaker(nullptr)
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
	_renderMgr->Init();

	_uiManager		= new UI::Manager::UIManager;
	_uiCamera		= new Camera::UICamera;
	_uiCamera->Initialize();

	_dx				= Device::Director::GetInstance()->GetDirectX();

	_backBufferMaker = new PostProcessing::BackBufferMaker;
	_backBufferMaker->Initialize();

	_lightManager	= new Manager::LightManager;

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		GET_CONTENT_FROM_ITERATOR(iter)->Update(dt);

	_lightManager->UpdateBuffer();

	_uiCamera->Update(dt);
}

void Scene::RenderPreview()
{
	OnRenderPreview();

	auto CamIteration = [&](Camera::CameraForm* cam)
	{
		cam->RenderPreviewWithUpdateTransformCB(_rootObjects);
	};

	_cameraMgr->IterateContent(CamIteration);
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
	
	ID3D11DeviceContext* context = _dx->GetContext();
	//Turn off depth writing
	context->OMSetDepthStencilState(_dx->GetDepthDisableDepthTestState(), 0);
	
	_uiCamera->Render();

	Camera::CameraForm* mainCam = _cameraMgr->GetMainCamera();
	_backBufferMaker->Render(_dx, mainCam, _uiCamera);

	//swap
	_dx->GetSwapChain()->Present(0, 0);

	OnRenderPost();
}

void Scene::Destroy()
{
	UI::FontLoader::GetInstance()->Destroy();

	SAFE_DELETE(_cameraMgr);
	SAFE_DELETE(_renderMgr);
	SAFE_DELETE(_uiCamera);
	SAFE_DELETE(_uiManager);
	SAFE_DELETE(_backBufferMaker);
	SAFE_DELETE(_lightManager);

	OnDestroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}

Core::Object* Scene::AddObject(Core::Object* object, bool clone)
{
	return _rootObjects.Add(object->GetName(), object, clone);
}

void Scene::Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(_state == State::Loop)
		OnInput(mouse, keyboard);
}