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

Shader::ShaderGroup RenderManager::LoadDefaultSahder(RenderType renderType, uint defaultVertexInputTypeFlag,
													 const std::string* customShaderFileName, const std::vector<ShaderMacro>* macros)
{
	std::string fileName = "";
	if(customShaderFileName ? customShaderFileName->empty() : true)
		MakeDefaultSahderFileName(fileName, renderType, defaultVertexInputTypeFlag);
	else
		fileName = (*customShaderFileName);

	std::hash_map<uint, const Shader::ShaderGroup>* repo = nullptr;
	std::vector<ShaderMacro> targetShaderMacros;

	if(macros)
		targetShaderMacros.assign(macros->begin(), macros->end());

	repo = &(_renderShaders[(uint)renderType]);

	if(renderType == RenderType::GBuffer_AlphaBlend || renderType == RenderType::Forward_MomentDepthWithAlphaTest)
	{
		ShaderMacro alphaTestMacro;
		alphaTestMacro.SetName("ENABLE_ALPHA_TEST");
		targetShaderMacros.push_back(alphaTestMacro);
	}
	if(renderType == RenderType::Forward_Transparency)// || renderType == RenderType::Forward_OnlyDepth)
	{
		ShaderMacro useTransparencyMacro;
		useTransparencyMacro.SetName("RENDER_TRANSPARENCY");
		targetShaderMacros.push_back(useTransparencyMacro);
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

		if(renderType == RenderType::Forward_AlphaTestWithDiffuse)
			psMainFunc = "OnlyAlpaTestWithDiffusePS";
		else if(renderType == RenderType::Voxelization)
			gsMainFunc = "GS";
		else if(renderType == RenderType::Forward_OnlyDepth)
		{
			vsMainFunc = "DepthOnlyVS";
			psMainFunc = "";
		}
		else if(renderType == RenderType::Forward_MomentDepth || renderType == RenderType::Forward_MomentDepthWithAlphaTest)
		{
			vsMainFunc = "MomentDepthVS";
			psMainFunc = "MomentDepthPS";
		}

		shader = LoadShader(fileName, vsMainFunc, psMainFunc, gsMainFunc, &targetShaderMacros, resourceManager->GetShaderManager());
		if(shader.IsAllEmpty() == false)
			repo->insert(std::make_pair(defaultVertexInputTypeFlag, shader));
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

	for(uint i=0; i< ((uint)RenderType::MAX_NUM); ++i)
	{
		RenderType type = (RenderType)i;

		LoadDefaultSahder(type, (uint)DefaultVertexInputTypeFlag::UV0, nullptr, &macros);

		LoadDefaultSahder(type,
			(uint)DefaultVertexInputTypeFlag::UV0 | 
			(uint)DefaultVertexInputTypeFlag::NORMAL, nullptr, &macros);

		LoadDefaultSahder(type,
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

bool RenderManager::FindShader(Shader::ShaderGroup& out, uint bufferFlag, RenderType renderType) const
{
	const auto& hashMap = _renderShaders[(uint)renderType];

	auto iter = hashMap.find(bufferFlag);
	if(iter == hashMap.end())
		return false;

	out = iter->second;
	return true;
}

bool RenderManager::HasShader(uint bufferFlag, RenderType renderType) const
{
	ShaderGroup dummy;
	return FindShader(dummy, bufferFlag, renderType);
}

void RenderManager::MakeDefaultSahderFileName(std::string& outFileName, RenderType renderType, uint bufferFlag) const
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
	
	switch(renderType)
	{
	case RenderType::Forward_AlphaTestWithDiffuse:
	case RenderType::Forward_OnlyDepth:
	case RenderType::Forward_Transparency:
	case RenderType::Forward_MomentDepth:
	case RenderType::Forward_MomentDepthWithAlphaTest:
		frontFileName = "PhysicallyBased_Forward_";
		break;
	case RenderType::GBuffer_AlphaBlend:
	case RenderType::GBuffer_Opaque:
		frontFileName = "PhysicallyBased_GBuffer_";
		break;
	case RenderType::Voxelization:
		frontFileName = "Voxelization_";
		break;
	default:
		ASSERT_MSG("Error, unsupported mesh type");
		break;
	}

	outFileName = frontFileName + defaultVertexInputTypeStr;
}