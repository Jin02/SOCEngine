#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;

Scene::Scene(void) : _cameraMgr(nullptr), _shaderMgr(nullptr), _textureMgr(nullptr), _materialMgr(nullptr), _sampler(nullptr), _meshImporter(nullptr), _bufferManager(nullptr), _originObjMgr(nullptr)
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
	_shaderMgr		= new Shader::ShaderManager;
	_textureMgr		= new Texture::TextureManager;
	_materialMgr	= new Material::MaterialManager;
	_meshImporter	= new Importer::MeshImporter;
	_bufferManager	= new Buffer::Manager;
	_originObjMgr	= new Core::ObjectManager;

	// Load Basic Shader
	{
		Shader::EngineFactory factory(_shaderMgr);

		//Basic. Only TexMapping
		if(factory.LoadShader(BASIC_SHADER_NAME, BASIC_VS_MAIN_FUNC_NAME, BASIC_PS_MAIN_FUNC_NAME, nullptr, nullptr) == false)
			ASSERT("Not Found Basic.hlsl");

		//Normal Mapping
		if(factory.LoadShader(BASIC_NORMAL_MAPPING_SHADER_NAME, BASIC_VS_MAIN_FUNC_NAME, BASIC_PS_MAIN_FUNC_NAME, nullptr, nullptr) == false)
			ASSERT("Not Found BasicNormalMapping.hlsl");
	}

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

void Scene::Render()
{
	Camera *mainCam = _cameraMgr->GetMainCamera();

	if(mainCam == nullptr)
		return;

	OnRenderPreview();

	mainCam->Render(_rootObjects);
		
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
 
	OnDestroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}