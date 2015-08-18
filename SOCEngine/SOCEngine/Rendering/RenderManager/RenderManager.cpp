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

Shader::ShaderGroup RenderManager::LoadDefaultSahder(MeshType meshType, uint defaultVertexInputTypeFlag,
													 const std::string* customShaderFileName, const std::vector<ShaderMacro>* macros)
{
	std::string fileName = "";
	if(customShaderFileName ? customShaderFileName->empty() : true)
	{
		std::string defaultVertexInputTypeStr = "";

		if(defaultVertexInputTypeFlag & (uint)DefaultVertexInputTypeFlag::N)
		{
			if(defaultVertexInputTypeFlag & (uint)DefaultVertexInputTypeFlag::TB)
				defaultVertexInputTypeStr += "TB";

			defaultVertexInputTypeStr += "N_";
		}

		if(defaultVertexInputTypeFlag & (uint)DefaultVertexInputTypeFlag::UV)
			defaultVertexInputTypeStr += "UV";

		std::string frontFileName = "";

		if(meshType == MeshType::Opaque || meshType == MeshType::AlphaTest)
			frontFileName = "PhysicallyBased_GBuffer_";
		else if(meshType == MeshType::Transparent)
			frontFileName = "PhysicallyBased_Transparency_";
		else
		{
			ASSERT_MSG("Error, unsupported mesh type");
		}

		fileName = frontFileName + defaultVertexInputTypeStr;
	}
	else
	{
		fileName = (*customShaderFileName);
	}

	std::hash_map<uint, const Shader::ShaderGroup>* repo = nullptr;
	std::vector<ShaderMacro>	targetShaderMacros;

	if(macros)
		targetShaderMacros.assign(macros->begin(), macros->end());

	if(meshType == MeshType::Opaque)
		repo = &_gbufferShaders;
	else if(meshType == MeshType::AlphaTest)
	{
		repo = &_gbufferShaders_alphaTest;

		if(macros)
		{
			ShaderMacro alphaTestMacro;
			alphaTestMacro.SetName("ENABLE_ALPHA_TEST");

			targetShaderMacros.push_back(alphaTestMacro);
		}

	}
	else if(meshType == MeshType::Transparent)
		repo = &_transparentShaders;
	else
	{
		ASSERT_MSG("Error, not supported mesh type");
	}

	auto iter = repo->find(defaultVertexInputTypeFlag);
	if(iter != repo->end())
		return iter->second;

	const ResourceManager* resourceManager = ResourceManager::GetInstance();

	auto LoadShader = [](const std::string& fileName, const std::vector<ShaderMacro>* macros, ShaderManager* shaderMgr)
	{
		Factory::EngineFactory shaderLoader(shaderMgr);

		ShaderGroup shaders;
		shaderLoader.LoadShader(fileName, "VS", "PS", macros, &shaders.vs, &shaders.ps);

		ASSERT_COND_MSG(shaders.vs, "RenderManager Error : can not load physically based material shader");
		return shaders;
	};

	ShaderGroup shader;
	{
		shader = LoadShader(fileName, &targetShaderMacros, resourceManager->GetShaderManager());
		repo->insert(std::make_pair(defaultVertexInputTypeFlag, shader));
	}

	return shader;
}

bool RenderManager::TestInit()
{
	std::vector<ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = Device::Director::GetInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);	
	}

	for(uint i=0; i< ((uint)MeshType::AlphaTest + 1); ++i)
	{
		LoadDefaultSahder((MeshType)i, (uint)DefaultVertexInputTypeFlag::UV, nullptr, &macros);
		LoadDefaultSahder((MeshType)i, (uint)DefaultVertexInputTypeFlag::UV | (uint)DefaultVertexInputTypeFlag::N, nullptr, &macros);
		LoadDefaultSahder((MeshType)i, (uint)DefaultVertexInputTypeFlag::UV | (uint)DefaultVertexInputTypeFlag::N | (uint)DefaultVertexInputTypeFlag::TB, nullptr, &macros);
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

bool RenderManager::FindGBufferShader(Shader::ShaderGroup& out, uint bufferFlag, bool isAlphaTest)
{
	auto FindObjectFromHashMap = [](Shader::ShaderGroup& outObject, const std::hash_map<uint, const Shader::ShaderGroup>& hashMap, uint key)
	{
		auto iter = hashMap.find(key);
		if(iter == hashMap.end())
			return false;

		outObject = iter->second;
		return true;
	};

	return FindObjectFromHashMap(out, isAlphaTest ? _gbufferShaders_alphaTest : _gbufferShaders, bufferFlag);
}

bool RenderManager::FindTransparencyShader(Shader::ShaderGroup& out, uint bufferFlag)
{
	auto iter = _transparentShaders.find(bufferFlag);
	if(iter == _transparentShaders.end())
		return false;

	out = iter->second;
	return true;
}