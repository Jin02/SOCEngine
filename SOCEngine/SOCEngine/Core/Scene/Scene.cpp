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
	_bufferManager(nullptr), _originObjMgr(nullptr), _renderMgr(nullptr)
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

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		GET_CONTENT_FROM_ITERATOR(iter)->Update(dt);
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
	OnRenderPost();
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