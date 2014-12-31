#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;

Scene::Scene(void) : 
	_cameraMgr(nullptr), _shaderMgr(nullptr), _textureMgr(nullptr), 
	_materialMgr(nullptr), _sampler(nullptr), _meshImporter(nullptr), 
	_bufferManager(nullptr), _originObjMgr(nullptr), _meshManager(nullptr)
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
	_shaderMgr		= new Manager::ShaderManager;
	_textureMgr		= new Manager::TextureManager;
	_materialMgr	= new Manager::MaterialManager;
	_meshImporter	= new Importer::MeshImporter;
	_bufferManager	= new Manager::BufferManager;
	_originObjMgr	= new Core::ObjectManager;
	_meshManager	= new Manager::RenderManager;

	_sampler = new Sampler;
	_sampler->Create();

	// Load Basic Shader
	{
		Factory::EngineFactory factory(_shaderMgr);

		auto LoadBasicMaterial = [&](const std::string& shaderName, const std::string& MaterialName,
			const std::string& vsMainFuncName, const std::string& psMainFuncName)
		{
			Rendering::Shader::VertexShader* vs = nullptr;			
			Rendering::Shader::PixelShader*	 ps = nullptr;

			const std::string includeFileName = "Common.hlsl";
			if(factory.LoadShader(shaderName, vsMainFuncName, psMainFuncName, &includeFileName, &vs, &ps) == false)
			{
				std::string error = "Not Found";
				error += shaderName + ".hlsl";
				ASSERT(error.c_str());
			}

			Material* material = new Material(MaterialName);
			material->SetVertexShader(vs);
			material->SetPixelShader(ps);
			_materialMgr->Add("Basic", MaterialName, material);
		};

		//Basic
		{
			const std::string tags[] = {"T0", "N_", "N_T0"};
			for(unsigned int i = 0; i <= ARRAYSIZE(tags); ++i)
			{
				std::string shaderName = BASIC_SHADER_NAME;
				if( i >= 1 )
					shaderName += tags[i-1];

				LoadBasicMaterial(shaderName, shaderName, BASIC_VS_MAIN_FUNC_NAME, BASIC_PS_MAIN_FUNC_NAME);
				LoadBasicMaterial(shaderName, shaderName + "_DepthWrite", DEPTH_WRITE_VS_MAIN_FUNC_NAME, DEPTH_WRITE_PS_MAIN_FUNC_NAME);
				LoadBasicMaterial(shaderName, shaderName + "_AlphaTest", ALPHA_TEST_VS_MAIN_FUNC_NAME, ALPHA_TEST_PS_MAIN_FUNC_NAME);
			}
		}

		//Normal Mapping
		//if(factory.LoadShader(BASIC_NORMAL_MAPPING_SHADER_NAME, BASIC_VS_MAIN_FUNC_NAME, BASIC_PS_MAIN_FUNC_NAME, nullptr, nullptr) == false)
		//	ASSERT("Not Found BasicNormalMapping.hlsl");

		//if(factory.LoadShader(BASIC_NORMAL_MAPPING_SHADER_NAME, DEPTH_WRITE_VS_MAIN_FUNC_NAME, DEPTH_WRITE_PS_MAIN_FUNC_NAME, nullptr, nullptr) == false)
		//	ASSERT("Not Found BasicNormalMapping_DepthWrite.hlsl");


		//Null Shader
		{
			_shaderMgr->Add("null:vs:NullVS", new Shader::VertexShader(nullptr));
			_shaderMgr->Add("null:vs:NullPS", new Shader::PixelShader(nullptr));
		}
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
	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();

	auto CamIteration = [&](Camera::Camera* cam)
	{
		cam->RenderObjects(dx, _meshManager);
	};

	_cameraMgr->IterateContent(CamIteration);

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
	SAFE_DELETE(_sampler);
	SAFE_DELETE(_meshManager);
 
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