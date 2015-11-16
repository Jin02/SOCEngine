#include "RenderManager.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"
#include "ResourceManager.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Resource;

RenderManager::RenderManager()
	: _opaqueMeshes(), _alphaBlendMeshes(), _transparentMeshes()
{
}

RenderManager::~RenderManager()
{
}

Shader::ShaderGroup RenderManager::LoadDefaultSahder(Geometry::MeshRenderer::Type meshType, uint defaultVertexInputTypeFlag,
													 const std::string* customShaderFileName, const std::vector<ShaderMacro>* macros)
{
	std::string fileName = "";
	if(customShaderFileName ? customShaderFileName->empty() : true)
		MakeDefaultSahderFileName(fileName, meshType, defaultVertexInputTypeFlag);
	else
		fileName = (*customShaderFileName);

	std::hash_map<uint, const Shader::ShaderGroup>* repo = nullptr;
	std::vector<ShaderMacro> targetShaderMacros;

	if(macros)
		targetShaderMacros.assign(macros->begin(), macros->end());

	if(meshType == Geometry::MeshRenderer::Type::Opaque)
		repo = &_gbufferShaders;
	else if(meshType == Geometry::MeshRenderer::Type::AlphaBlend)
	{
		repo = &_gbufferShaders_alphaTest;

		ShaderMacro alphaTestMacro;
		alphaTestMacro.SetName("ENABLE_ALPHA_TEST");
		targetShaderMacros.push_back(alphaTestMacro);
	}
	else if(meshType == Geometry::MeshRenderer::Type::Transparent)
	{
		repo = &_forward_transparentShaders;

		ShaderMacro useTransparencyMacro;
		useTransparencyMacro.SetName("RENDER_TRANSPARENCY");
		targetShaderMacros.push_back(useTransparencyMacro);
	}
	else if(meshType == Geometry::MeshRenderer::Type::OnlyAlphaTestWithDiffuse)
		repo = &_forward_onlyAlphaTestWithDiffuseShaders;
	else if(meshType == Geometry::MeshRenderer::Type::Voxelization)
		repo = &_voxelizationShaders;
	else
	{
		ASSERT_MSG("Error, not supported mesh type");
	}

	auto iter = repo->find(defaultVertexInputTypeFlag);
	if(iter != repo->end())
		return iter->second;

	const ResourceManager* resourceManager = ResourceManager::GetInstance();

	auto LoadShader = [](
		const std::string& fileName,
		const std::string& vsMainName, const std::string& psMainName, const std::string& gsMainName,
		const std::vector<ShaderMacro>* macros, ShaderManager* shaderMgr) -> ShaderGroup
	{
		Factory::EngineFactory shaderLoader(shaderMgr);

		ShaderGroup shaders;
		shaderLoader.LoadShader(fileName, vsMainName, psMainName, gsMainName, macros, &shaders.vs, &shaders.ps, &shaders.gs);

		//ASSERT_COND_MSG(shaders.vs, "RenderManager Error : can not load shader");
		return shaders;
	};

	ShaderGroup shader;
	{
		std::string vsMainFunc = "VS", psMainFunc ="PS", gsMainFunc = "";

		if(meshType == Geometry::MeshRenderer::Type::OnlyAlphaTestWithDiffuse)
			psMainFunc = "OnlyAlpaTestWithDiffusePS";
		else if(meshType == Geometry::MeshRenderer::Type::Voxelization)
			gsMainFunc = "GS";

		shader = LoadShader(fileName, vsMainFunc, psMainFunc, gsMainFunc, &targetShaderMacros, resourceManager->GetShaderManager());
		if(shader.IsAllEmpty() == false)
		{
			repo->insert(std::make_pair(defaultVertexInputTypeFlag, shader));

			if(meshType == Geometry::MeshRenderer::Type::Transparent)
			{
				shader = LoadShader(fileName, "DepthOnlyVS", "", "", &targetShaderMacros, resourceManager->GetShaderManager());
				_forward_depthOnlyShaders.insert(std::make_pair(defaultVertexInputTypeFlag, shader));
			}
		}
	}

	return shader;
}

void RenderManager::Initialize()
{
	std::vector<ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = Device::Director::GetInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);	
	}

	for(uint i=0; i< ((uint)Geometry::MeshRenderer::Type::MAX); ++i)
	{
		LoadDefaultSahder((Geometry::MeshRenderer::Type)i,
			(uint)DefaultVertexInputTypeFlag::UV0, nullptr, &macros);

		LoadDefaultSahder((Geometry::MeshRenderer::Type)i,
			(uint)DefaultVertexInputTypeFlag::UV0 | 
			(uint)DefaultVertexInputTypeFlag::NORMAL, nullptr, &macros);

		LoadDefaultSahder((Geometry::MeshRenderer::Type)i,
			(uint)DefaultVertexInputTypeFlag::UV0 | 
			(uint)DefaultVertexInputTypeFlag::NORMAL | 
			(uint)DefaultVertexInputTypeFlag::TANGENT, nullptr, &macros);
	}

	DEBUG_LOG("Done Loading Shaders!")
}

void RenderManager::UpdateRenderList(const Geometry::Mesh* mesh)
{
	MeshList::meshkey meshAddress = reinterpret_cast<MeshList::meshkey>(mesh);

	auto GetMeshList = [&](Geometry::MeshRenderer::Type renderType)
	{
		MeshList* meshList = nullptr;

		if( renderType == Geometry::MeshRenderer::Type::Transparent )
			meshList = &_transparentMeshes;
		else if( renderType == Geometry::MeshRenderer::Type::Opaque )
			meshList = &_opaqueMeshes;
		else if( renderType == Geometry::MeshRenderer::Type::AlphaBlend )
			meshList = &_alphaBlendMeshes;
		else
			ASSERT_MSG("Error, unsupported mesh type");
		
		return meshList;
	};

	Geometry::MeshRenderer::Type currentType	= mesh->GetMeshRenderer()->GetCurrentRenderType();
	Geometry::MeshRenderer::Type prevType		= mesh->GetPrevRenderType();

	if(prevType == currentType)
		return; // not changed

	const std::string& vbKey = mesh->GetMeshFilter()->GetVertexBuffer()->GetKey();

	if(prevType != Geometry::MeshRenderer::Type::Unknown)
	{
		MeshList* prevMeshList = GetMeshList(prevType);

		std::set<MeshList::meshkey>* meshSets = prevMeshList->meshes.Find(vbKey);
		if(meshSets)
		{
			meshSets->erase(meshAddress);

			if(meshSets->empty())
				prevMeshList->meshes.Delete(vbKey);

			++prevMeshList->updateCounter;
		}
	}

	if(currentType != Geometry::MeshRenderer::Type::Unknown)
	{
		MeshList* currentMeshList = GetMeshList(currentType);
		std::set<MeshList::meshkey>* meshSets = currentMeshList->meshes.Find(vbKey);
		if(meshSets == nullptr)
		{
			std::set<MeshList::meshkey> set;
			set.insert(meshAddress);

			currentMeshList->meshes.Add(vbKey, set);
		}
		else
		{
			meshSets->insert(meshAddress);
		}
		++currentMeshList->updateCounter;
	}
}

bool RenderManager::HasMeshInRenderList(const Geometry::Mesh* mesh, Geometry::MeshRenderer::Type type)
{
	MeshList::meshkey meshAddress = reinterpret_cast<MeshList::meshkey>(mesh);
	const Geometry::Mesh* foundedMesh = nullptr;
	const std::string& vbKey = mesh->GetMeshFilter()->GetVertexBuffer()->GetKey();

	MeshList* meshList = nullptr;

	if(type == Geometry::MeshRenderer::Type::Transparent)
		meshList = &_transparentMeshes;
	else if(type == Geometry::MeshRenderer::Type::Opaque)
		meshList = &_opaqueMeshes;
	else if(type == Geometry::MeshRenderer::Type::AlphaBlend)
		meshList = &_alphaBlendMeshes;
	else
	{
		ASSERT_MSG("Error!, undefined Geometry::MeshRenderer::Type");
	}

	std::set<MeshList::meshkey>* meshSets = meshList->meshes.Find(vbKey);
	if(meshSets)
	{
		auto foundIter = meshSets->find(meshAddress);
		foundedMesh = (foundIter != meshSets->end()) ? reinterpret_cast<Geometry::Mesh*>(*foundIter) : nullptr;
	}

	return foundedMesh != nullptr;
}

bool RenderManager::FindGBufferShader(Shader::ShaderGroup& out, uint bufferFlag, bool isAlphaTest) const
{	
	return FindShaderFromHashMap(out, isAlphaTest ? _gbufferShaders_alphaTest : _gbufferShaders, bufferFlag);
}

bool RenderManager::FindTransparencyShader(Shader::ShaderGroup& out, uint bufferFlag) const
{
	return FindShaderFromHashMap(out, _forward_transparentShaders, bufferFlag);
}

bool RenderManager::FindDepthOnlyShader(Shader::ShaderGroup& out, uint bufferFlag) const
{
	return FindShaderFromHashMap(out, _forward_depthOnlyShaders, bufferFlag);
}

bool RenderManager::FindOnlyAlphaTestWithDiffuseShader(Shader::ShaderGroup& out, uint bufferFlag) const
{
	return FindShaderFromHashMap(out, _forward_onlyAlphaTestWithDiffuseShaders, bufferFlag);
}

bool RenderManager::FindShaderFromHashMap(Shader::ShaderGroup& outObject, const std::hash_map<uint, const Shader::ShaderGroup>& hashMap, uint key) const
{
	auto iter = hashMap.find(key);
	if(iter == hashMap.end())
		return false;

	outObject = iter->second;
	return true;
}

bool RenderManager::HasGBufferShader(uint bufferFlag, bool isAlphaTest) const
{
	ShaderGroup dummy;
	return FindGBufferShader(dummy, bufferFlag, isAlphaTest);
}

bool RenderManager::HasTransparencyShader(uint bufferFlag) const
{
	ShaderGroup dummy;
	return FindTransparencyShader(dummy, bufferFlag);
}

bool RenderManager::HasDepthOnlyShader(uint bufferFlag) const
{
	ShaderGroup dummy;
	return FindDepthOnlyShader(dummy, bufferFlag);
}

bool RenderManager::HasDepthOnlyShaderOnlyAlphaTestWithDiffuseShader(uint bufferFlag) const
{
	ShaderGroup dummy;
	return FindOnlyAlphaTestWithDiffuseShader(dummy, bufferFlag);
}

void RenderManager::MakeDefaultSahderFileName(
	std::string& outFileName,
	Geometry::MeshRenderer::Type meshType, uint bufferFlag) const
{
	std::string defaultVertexInputTypeStr = "";

	if(bufferFlag & (uint)DefaultVertexInputTypeFlag::NORMAL)
	{
		if(bufferFlag & (uint)DefaultVertexInputTypeFlag::TANGENT)
			defaultVertexInputTypeStr += "T";

		defaultVertexInputTypeStr += "N_";
	}

	if(bufferFlag & (uint)DefaultVertexInputTypeFlag::UV0)
		defaultVertexInputTypeStr += "UV0";

	std::string frontFileName = "";

	if( meshType == Geometry::MeshRenderer::Type::Opaque || 
		meshType == Geometry::MeshRenderer::Type::AlphaBlend )
		frontFileName = "PhysicallyBased_GBuffer_";
	else if(meshType == Geometry::MeshRenderer::Type::Transparent ||
			meshType == Geometry::MeshRenderer::Type::OnlyAlphaTestWithDiffuse)
		frontFileName = "PhysicallyBased_Forward_";
	else if(meshType == Geometry::MeshRenderer::Type::Voxelization)
		frontFileName = "Voxelization_";
	else
	{
		ASSERT_MSG("Error, unsupported mesh type");
	}

	outFileName = frontFileName + defaultVertexInputTypeStr;
}