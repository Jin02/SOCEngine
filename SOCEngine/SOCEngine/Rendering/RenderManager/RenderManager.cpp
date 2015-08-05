#include "RenderManager.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"
#include "ResourceManager.h"

using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Resource;

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{
}

bool RenderManager::Init()
{
	const ResourceManager* resourceManager = ResourceManager::GetInstance();
	ShaderManager* shaderMgr = resourceManager->GetShaderManager();

	Factory::EngineFactory shaderLoader(shaderMgr);

	//Physically Based Shading
	{
		auto LoadGBuffer = [&](const std::vector<std::string>& macros, std::hash_map<Mesh::MeshFilter::BufferElementFlag, const ShaderGroup>& out)
		{
			const std::string shaderFileNameTypes[] = {"UV", "N_UV", "TBN_UV"};
			const Mesh::MeshFilter::BufferElementFlag bufferElems[] = {
				(uint)Mesh::MeshFilter::BufferElement::UV,

				(uint)Mesh::MeshFilter::BufferElement::UV | 
				(uint)Mesh::MeshFilter::BufferElement::Normal,

				(uint)Mesh::MeshFilter::BufferElement::Normal | 
				(uint)Mesh::MeshFilter::BufferElement::Tangent | 
				(uint)Mesh::MeshFilter::BufferElement::Binormal | 
				(uint)Mesh::MeshFilter::BufferElement::UV
			};

			const std::string frontFileName = "PhysicallyBased_GBuffer_";
			const std::string includeFileName = "PhysicallyBased_GBuffer_Common";

			auto LoadShader = [&](const std::string& fileName, const std::string& vsFuncName, const std::string& psFuncName, const std::vector<std::string>& macros)
			{
				ShaderGroup shaders;
				shaderLoader.LoadShader(fileName, vsFuncName, psFuncName, &includeFileName, &macros, &shaders.vs, &shaders.ps);

				ASSERT_COND_MSG(shaders.vs, "RenderManager Error : can not load physically based material shader");
				return shaders;
			};

			ShaderGroup shader;
			for(int i = 0; i < ARRAYSIZE(shaderFileNameTypes); ++i)
			{
				std::string fileName = frontFileName + shaderFileNameTypes[i];

				shader = LoadShader(fileName, "VS", "PS", macros);
				out.insert(std::make_pair(bufferElems[i], shader));
			}
		};

		std::vector<std::string> macros;
		LoadGBuffer(macros, _gbufferShaders);

		macros.push_back("#define ENABLE_ALPHA_TEST");
		LoadGBuffer(macros, _gbufferShaders_alphaTest);
	}

	return true;
}

void RenderManager::UpdateRenderList(const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);
	MeshList* meshList = nullptr;

	if( type == MeshType::Transparent )
		meshList = &_transparentMeshes;
	else if( type == MeshType::Opaque )
		meshList = &_opaqueMeshes;
	else if( type == MeshType::AlphaTest )
		meshList = &_alphaTestMeshes;
	else
	{
		ASSERT_MSG("Error, unsupported mesh type");
	}

	meshList->meshes.Delete(meshAddress);

	if(meshList->meshes.Find(meshAddress) == nullptr)
		meshList->meshes.Add(meshAddress, mesh);

	++(meshList->updateCounter);
}

const Mesh::Mesh* RenderManager::FindMeshFromRenderList(const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if(type == MeshType::Transparent)
		return (*_transparentMeshes.meshes.Find(meshAddress));
	else if(type == MeshType::Opaque)
		return (*_opaqueMeshes.meshes.Find(meshAddress));
	else if(type == MeshType::AlphaTest)
		return (*_alphaTestMeshes.meshes.Find(meshAddress));
	else
		ASSERT_MSG("Error!, undefined MeshType");

	return nullptr;
}

bool RenderManager::FindGBufferShader(Shader::ShaderGroup& out, Mesh::MeshFilter::BufferElementFlag bufferFlag, bool isAlphaTest)
{
	auto FindObjectFromHashMap = [](Shader::ShaderGroup& outObject, const std::hash_map<Mesh::MeshFilter::BufferElementFlag, const Shader::ShaderGroup>& hashMap, Mesh::MeshFilter::BufferElementFlag key)
	{
		auto iter = hashMap.find(key);
		if(iter == hashMap.end())
			return false;

		outObject = iter->second;
		return true;
	};

	return FindObjectFromHashMap(out, isAlphaTest ? _gbufferShaders_alphaTest : _gbufferShaders ,bufferFlag);
}