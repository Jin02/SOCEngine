#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"
#include "FontLoader.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::PostProcessing;
using namespace Rendering::TBDR;
using namespace Rendering::Shader;
using namespace UI::Manager;

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
	_cameraMgr		= new CameraManager;

	_renderMgr		= new RenderManager;
	_renderMgr->TestInit();

	_uiManager		= new UIManager;

	_dx				= Device::Director::GetInstance()->GetDirectX();

	_lightManager	= new LightManager;	
	_lightManager->InitializeAllShaderResourceBuffer();

	_materialMgr	= new MaterialManager;


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

	auto materials = _materialMgr->GetMaterials().GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		(*iter)->UpdateConstBuffer(_dx);

	const std::vector<Camera::CameraForm*>& cameras = _cameraMgr->GetVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		(*iter)->UpdateTransformCB(_rootObjects.GetVector());
}

void Scene::Render()
{
	const std::vector<Camera::CameraForm*>& cameras = _cameraMgr->GetVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		(*iter)->Render(_dx, _renderMgr, _lightManager);



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
	SAFE_DELETE(_materialMgr);

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

Core::Object* Scene::FindObject(const std::string& key)
{
	Core::Object** objAddr = _rootObjects.Find(key);
	return objAddr ? (*objAddr) : nullptr;
}

void Scene::DeleteObject(Core::Object* object)
{
	std::string key = object->GetName();
	Core::Object** objectAddr = _rootObjects.Find(key);
	if(objectAddr)
	{
		Core::Object* object = (*objectAddr);
		SAFE_DELETE(object);

		_rootObjects.Delete(key);
	}
}

void Scene::DeleteAllObject()
{
	auto& objects = _rootObjects.GetVector();
	for(auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		Core::Object* object = (*iter);
		SAFE_DELETE(object);
	}

	_rootObjects.DeleteAll();
}

void Scene::Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(_state == State::Loop)
		OnInput(mouse, keyboard);
}