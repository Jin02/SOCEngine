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

Shader::ShaderGroup RenderManager::LoadDefaultSahder(Mesh::MeshRenderer::Type meshType, uint defaultVertexInputTypeFlag,
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

		if(meshType == Mesh::MeshRenderer::Type::Opaque || meshType == Mesh::MeshRenderer::Type::AlphaBlend)
			frontFileName = "PhysicallyBased_GBuffer_";
		else if(meshType == Mesh::MeshRenderer::Type::Transparent)
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

	if(meshType == Mesh::MeshRenderer::Type::Opaque)
		repo = &_gbufferShaders;
	else if(meshType == Mesh::MeshRenderer::Type::AlphaBlend)
	{
		repo = &_gbufferShaders_alphaTest;

		ShaderMacro alphaTestMacro;
		alphaTestMacro.SetName("ENABLE_ALPHA_TEST");
		targetShaderMacros.push_back(alphaTestMacro);
	}
	else if(meshType == Mesh::MeshRenderer::Type::Transparent)
	{
		repo = &_transparentShaders;

		ShaderMacro useTransparencyMacro;
		useTransparencyMacro.SetName("RENDER_TRANSPARENCY");
		targetShaderMacros.push_back(useTransparencyMacro);
	}
	else
	{
		ASSERT_MSG("Error, not supported mesh type");
	}

	auto iter = repo->find(defaultVertexInputTypeFlag);
	if(iter != repo->end())
		return iter->second;

	const ResourceManager* resourceManager = ResourceManager::GetInstance();

	auto LoadShader = [](const std::string& fileName, const std::string& vsMainName, const std::string& psMainName, const std::vector<ShaderMacro>* macros, ShaderManager* shaderMgr)
	{
		Factory::EngineFactory shaderLoader(shaderMgr);

		ShaderGroup shaders;
		shaderLoader.LoadShader(fileName, vsMainName, psMainName, macros, &shaders.vs, &shaders.ps);

		ASSERT_COND_MSG(shaders.vs, "RenderManager Error : can not load physically based material shader");
		return shaders;
	};

	ShaderGroup shader;
	{
		shader = LoadShader(fileName, "VS", "PS", &targetShaderMacros, resourceManager->GetShaderManager());
		repo->insert(std::make_pair(defaultVertexInputTypeFlag, shader));

		if(meshType == Mesh::MeshRenderer::Type::Transparent)
		{
			shader = LoadShader(fileName, "DepthOnlyVS", "", &targetShaderMacros, resourceManager->GetShaderManager());
			_transparent_depthOnly_Shaders.insert(std::make_pair(defaultVertexInputTypeFlag, shader));
		}
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

	for(uint i=0; i< ((uint)Mesh::MeshRenderer::Type::AlphaBlend + 1); ++i)
	{
		LoadDefaultSahder((Mesh::MeshRenderer::Type)i, (uint)DefaultVertexInputTypeFlag::UV, nullptr, &macros);
		LoadDefaultSahder((Mesh::MeshRenderer::Type)i, (uint)DefaultVertexInputTypeFlag::UV | (uint)DefaultVertexInputTypeFlag::N, nullptr, &macros);
		LoadDefaultSahder((Mesh::MeshRenderer::Type)i, (uint)DefaultVertexInputTypeFlag::UV | (uint)DefaultVertexInputTypeFlag::N | (uint)DefaultVertexInputTypeFlag::TB, nullptr, &macros);
	}

	return true;
}

void RenderManager::UpdateRenderList(const Mesh::Mesh* mesh)
{
	MeshList::meshkey meshAddress = reinterpret_cast<MeshList::meshkey>(mesh);

	auto GetMeshList = [&](Mesh::MeshRenderer::Type renderType)
	{
		MeshList* meshList = nullptr;

		if( renderType == Mesh::MeshRenderer::Type::Transparent )
			meshList = &_transparentMeshes;
		else if( renderType == Mesh::MeshRenderer::Type::Opaque )
			meshList = &_opaqueMeshes;
		else if( renderType == Mesh::MeshRenderer::Type::AlphaBlend )
			meshList = &_alphaBlendMeshes;
		else
			ASSERT_MSG("Error, unsupported mesh type");
		
		return meshList;
	};

	Mesh::MeshRenderer::Type currentType	= mesh->GetMeshRenderer()->GetCurrentRenderType();
	Mesh::MeshRenderer::Type prevType		= mesh->GetPrevRenderType();

	if(prevType == currentType)
		return; // not changed

	const std::string& vbKey = mesh->GetMeshFilter()->GetVertexBuffer()->GetKey();

	MeshList* prevMeshList = GetMeshList(prevType);
	{
		MeshList::MeshesMap* list = prevMeshList->meshes.Find(vbKey);
		if(list)
		{
			list->Delete(meshAddress);

			if(list->GetSize() == 0)
				prevMeshList->meshes.Delete(vbKey);

			++prevMeshList->updateCounter;
		}
	}

	MeshList* currentMeshList = GetMeshList(currentType);
	{
		MeshList::MeshesMap* list = currentMeshList->meshes.Find(vbKey);
		if(list == nullptr)
		{
			MeshList::MeshesMap mm;
			mm.Add(meshAddress, mesh);

			currentMeshList->meshes.Add(vbKey, mm);
		}
		else
		{
			list->Add(meshAddress, mesh);
		}

		++currentMeshList->updateCounter;
	}
}

bool RenderManager::HasMeshInRenderList(const Mesh::Mesh* mesh, Mesh::MeshRenderer::Type type)
{
	MeshList::meshkey meshAddress = reinterpret_cast<MeshList::meshkey>(mesh);
	const Mesh::Mesh* foundedMesh = nullptr;
	const std::string& vbKey = mesh->GetMeshFilter()->GetVertexBuffer()->GetKey();

	MeshList* meshList = nullptr;

	if(type == Mesh::MeshRenderer::Type::Transparent)
		meshList = &_transparentMeshes;
	else if(type == Mesh::MeshRenderer::Type::Opaque)
		meshList = &_opaqueMeshes;
	else if(type == Mesh::MeshRenderer::Type::AlphaBlend)
		meshList = &_alphaBlendMeshes;
	else
	{
		ASSERT_MSG("Error!, undefined Mesh::MeshRenderer::Type");
	}

	MeshList::MeshesMap* meshesMap = meshList->meshes.Find(vbKey);
	if(meshesMap)
	{
		const Mesh::Mesh** found = meshesMap->Find(meshAddress);
		foundedMesh = found ? (*found) : nullptr;
	}

	return foundedMesh != nullptr;
}

bool RenderManager::FindGBufferShader(Shader::ShaderGroup& out, uint bufferFlag, bool isAlphaTest) const
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

bool RenderManager::FindTransparencyShader(Shader::ShaderGroup& out, uint bufferFlag, bool isDepthOnly) const
{
	auto iter = isDepthOnly ?	_transparent_depthOnly_Shaders.find(bufferFlag) : 
								_transparentShaders.find(bufferFlag);

	if(iter == _transparentShaders.end())
		return false;

	out = iter->second;
	return true;
}