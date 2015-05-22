#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;

Scene::Scene(void) : 
	_cameraMgr(nullptr), _shaderMgr(nullptr), _textureMgr(nullptr), 
	_materialMgr(nullptr), _meshImporter(nullptr), 	_uiManager(nullptr),
	_bufferManager(nullptr), _originObjMgr(nullptr), _renderMgr(nullptr),
	_uiCamera(nullptr)
{
	_state = State::Init;
}

Scene::~Scene(void)
{
	Destroy();
}

void Scene::Initialize()
{
	_shaderMgr		= new Manager::ShaderManager;
	_textureMgr		= new Manager::TextureManager;
	_materialMgr	= new Manager::MaterialManager;
	_meshImporter	= new Importer::MeshImporter;
	_bufferManager	= new Manager::BufferManager;
	_originObjMgr	= new Core::ObjectManager;

	_cameraMgr		= new Manager::CameraManager;
	_cameraMgr->InitLightCulling();

	_renderMgr		= new Manager::RenderManager;
	_renderMgr->Init();

	_uiManager		= new UI::Manager::UIManager;
	_uiCamera		= new Camera::UICamera;
	_uiCamera->Initialize();

	_dx				= Device::Director::GetInstance()->GetDirectX();

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		GET_CONTENT_FROM_ITERATOR(iter)->Update(dt);

	_uiCamera->Update(dt);
}

void Scene::RenderPreview()
{
	OnRenderPreview();

	auto CamIteration = [&](Camera::Camera* cam)
	{
		cam->UpdateTransformCBAndCheckRender(_rootObjects);
	};

	_cameraMgr->IterateContent(CamIteration);
}

void Scene::Render()
{
	auto CamIteration = [&](Camera::Camera* cam)
	{
		cam->Render();
	};
	_cameraMgr->IterateContent(CamIteration);

	OnRenderPost();

	//ui
	{
		_uiCamera->Render();
	}

	//swap
	_dx->GetSwapChain()->Present(0, 0);
}

void Scene::Destroy()
{
	SAFE_DELETE(_cameraMgr);
	SAFE_DELETE(_shaderMgr);
	SAFE_DELETE(_textureMgr);
	SAFE_DELETE(_materialMgr);
	SAFE_DELETE(_meshImporter);
	SAFE_DELETE(_bufferManager);
	SAFE_DELETE(_originObjMgr);
	SAFE_DELETE(_renderMgr);
	SAFE_DELETE(_uiCamera);
	SAFE_DELETE(_uiManager);

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