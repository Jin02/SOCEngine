#include "RenderManager.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Manager;

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
	_alphaMeshes.DeleteAll(true);
	_nonAlphaMeshes.DeleteAll(true);
}

bool RenderManager::Init()
{
	const Core::Scene*		currentScene	= Device::Director::GetInstance()->GetCurrentScene();
	Manager::ShaderManager* shaderMgr		= currentScene->GetShaderManager();

	Factory::EngineFactory shaderLoader(shaderMgr);

	//Physically Based GBuffer
	{
		const std::string keys[] = {"N", "N_T0", "T0", "TBN_T0"};

		const std::string frontFileName = "PhysicallyBased_GBuffer_";
		const std::string includeFileName = "PhysicallyBased_GBuffer_Common";

		for(int i = 0; i < ARRAYSIZE(keys); ++i)
		{
			std::string fileName = frontFileName + keys[i];
			
			Shaders shaders;
			bool loadSuccess = shaderLoader.LoadShader(fileName, "VS", "PS", &includeFileName, &shaders.vs, &shaders.ps);

			ASSERT_COND_MSG(loadSuccess, "RenderManager Error : can not load physically based geometry buffer shader");
			{
				shaderMgr->RemoveShaderCode(fileName+":vs:VS");
				shaderMgr->RemoveShaderCode(fileName+":vs:PS");
			}
		}
	}

	//Basic GBuffer
	{
		//const std::string keys[] = {"N", "N_T0", "T0", "TBN_T0"};

		//const std::string frontFileName = "GBuffer_";
		//const std::string includeFileName = "GBuffer_Common";

		//for(int i = 0; i < ARRAYSIZE(keys); ++i)
		//{
		//	std::string fileName = frontFileName + keys[i];
		//	
		//	Shaders shaders;
		//	bool loadSuccess = shaderLoader.LoadShader(fileName, "VS", "PS", &includeFileName, &shaders.vs, &shaders.ps);
		//	ASSERT_COND_MSG(loadSuccess, "RenderManager Error : can not load physically based geometry buffer shader");
		//}
	}

	return true;
}

bool RenderManager::Add(Material* material, Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	std::pair<Material*, Mesh::Mesh*>* pair = Find(material, mesh, type);
	if(pair == nullptr)
		pair = new std::pair<Material*, Mesh::Mesh*>(material, mesh);

	if(type == MeshType::hasAlpha)
		_alphaMeshes.Add(materialAddress, meshAddress, pair);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.Add(materialAddress, meshAddress, pair);
	else
	{
		DEBUG_LOG("undeclartion MeshType");
		return false;
	}

	return true;
}

void RenderManager::Change(const Material* material, const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if( type == MeshType::hasAlpha )
	{
		auto pair = _nonAlphaMeshes.Find(materialAddress, meshAddress);
		_nonAlphaMeshes.Delete(materialAddress, meshAddress, false);

		_alphaMeshes.Add(materialAddress, meshAddress, pair);
	}
	else if( type == MeshType::nonAlpha )
	{
		auto pair = _alphaMeshes.Find(materialAddress, meshAddress);
		_alphaMeshes.Delete(materialAddress, meshAddress, false);

		_nonAlphaMeshes.Add(materialAddress, meshAddress, pair);
	}
}

std::pair<Material*, Mesh::Mesh*>* RenderManager::Find(Material* material, Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);
	
	if(type == MeshType::hasAlpha)
		return _alphaMeshes.Find(materialAddress, meshAddress);
	else if(type == MeshType::nonAlpha)
		return _nonAlphaMeshes.Find(materialAddress, meshAddress);
	else
		ASSERT_MSG("Error!, undefined MeshType");

	return nullptr;
}

void RenderManager::Iterate(const std::function<void(Material* material, Mesh::Mesh* mesh)>& recvFunc, MeshType type) const
{
	auto MapInMapIter = [&](Structure::Map<unsigned int, std::pair<Material*, Mesh::Mesh*>>* content)
	{
		auto MapIter = [&](std::pair<Material*, Mesh::Mesh*>* content)
		{
			recvFunc(content->first, content->second);
		};
		content->IterateContent(MapIter);
	};

	if(type == MeshType::hasAlpha)
		_alphaMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else
	{
		ASSERT_MSG("Error!, undefined MeshType");
	}
}

void RenderManager::Render(const Camera::Camera* camera)
{
	auto NonAlphaMeshRender = [](Material* material, Mesh::Mesh* mesh)
	{

	};
	Iterate(NonAlphaMeshRender, MeshType::nonAlpha);
}