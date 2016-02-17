#include "RenderManager.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"
#include "ResourceManager.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Resource;
using namespace Rendering::Geometry;

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

	const ResourceManager* resourceManager = ResourceManager::SharedInstance();

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
		std::vector<ShaderMainFuncName> mainFuncNames;
		MakeDefaultShaderMainFuncNames(mainFuncNames, renderType);

		std::string vsMainFunc = "", gsMainFunc = "", psMainFunc = "";
		for(auto iter = mainFuncNames.begin(); iter != mainFuncNames.end(); ++iter)
		{
			if(iter->type == ShaderForm::Type::Vertex)			vsMainFunc = iter->name;
			else if(iter->type == ShaderForm::Type::Pixel)		psMainFunc = iter->name;
			else if(iter->type == ShaderForm::Type::Geometry)	gsMainFunc = iter->name;
			else												ASSERT_MSG("Error, Unsupported Type");
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
		ShaderMacro msaaMacro = Device::Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);	
	}

	for(uint i=0; i< ((uint)RenderType::MAX_NUM); ++i)
	{
		RenderType type = (RenderType)i;

		if(type != RenderType::Voxelization)
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

void RenderManager::ClearMeshList(MeshRenderer::Type type)
{
	MeshList* targetList = nullptr;

	if(type == MeshRenderer::Type::Opaque)
		targetList = &_opaqueMeshes;
	else if(type == MeshRenderer::Type::AlphaBlend)
		targetList = &_alphaBlendMeshes;
	else if(type == MeshRenderer::Type::Transparent)
		targetList = &_transparentMeshes;
	else
	{
		ASSERT_MSG("Error, Unsupported mesh type.");
	}

	targetList->updateCounter = 0xffffffff;
	targetList->meshes.DeleteAll();
}

void RenderManager::ClearAllMeshList()
{
	ClearMeshList(MeshRenderer::Type::Opaque);
	ClearMeshList(MeshRenderer::Type::Transparent);
	ClearMeshList(MeshRenderer::Type::AlphaBlend);
}

void RenderManager::MakeDefaultShaderMainFuncNames(std::vector<ShaderMainFuncName>& outMainFuncNames, RenderType renderType)
{
	std::string vsMain = "VS";
	std::string psMain = "PS";
	std::string gsMain = "";

	if(renderType == RenderType::Forward_AlphaTestWithDiffuse)
		psMain = "OnlyAlpaTestWithDiffusePS";
	else if(renderType == RenderType::Voxelization)
		gsMain = "GS";
	else if(renderType == RenderType::Forward_OnlyDepth)
	{
		vsMain = "DepthOnlyVS";
		psMain = "";
	}
	else if(renderType == RenderType::Forward_MomentDepth || renderType == RenderType::Forward_MomentDepthWithAlphaTest)
	{
		vsMain = "MomentDepthVS";
		psMain = "MomentDepthPS";
	}

	ShaderMainFuncName pair;
	pair.type	= ShaderForm::Type::Vertex;
	pair.name	= vsMain;
	outMainFuncNames.push_back(pair);

	pair.type	= ShaderForm::Type::Geometry;
	pair.name	= gsMain;
	outMainFuncNames.push_back(pair);

	pair.type	= ShaderForm::Type::Pixel;
	pair.name	= psMain;
	outMainFuncNames.push_back(pair);
}

void RenderManager::DeleteDefaultShader(RenderType renderType)
{
	const ResourceManager* resourceManager = ResourceManager::SharedInstance();
	ShaderManager* shaderMgr = resourceManager->GetShaderManager();

	auto DeleteShaderByBufferFlag = [&](uint bufferFlag)
	{
		std::string shaderName;
		MakeDefaultSahderFileName(shaderName, renderType, bufferFlag);

		std::vector<ShaderMainFuncName> mainFuncNames;
		MakeDefaultShaderMainFuncNames(mainFuncNames, renderType);

		std::string vsMainFunc = "", gsMainFunc = "", psMainFunc = "";
		for(auto iter = mainFuncNames.begin(); iter != mainFuncNames.end(); ++iter)
		{
			ShaderForm::Type shaderType = iter->type;
			const std::string& mainFunc = iter->name;

			std::string shaderTypeStr = "";
			if(shaderType == ShaderForm::Type::Vertex)			shaderTypeStr = "vs";
			else if(shaderType == ShaderForm::Type::Geometry)	shaderTypeStr = "gs";
			else if(shaderType == ShaderForm::Type::Pixel)		shaderTypeStr = "ps";
			else ASSERT_MSG("Error, Unsupported shader type");

			std::string command = shaderMgr->MakeFullCommand(shaderName, mainFunc, shaderTypeStr);
			shaderMgr->DeleteShader(command);	
		}
	};

	DeleteShaderByBufferFlag((uint)DefaultVertexInputTypeFlag::UV0);
	DeleteShaderByBufferFlag((uint)DefaultVertexInputTypeFlag::UV0 | (uint)DefaultVertexInputTypeFlag::NORMAL);
	DeleteShaderByBufferFlag((uint)DefaultVertexInputTypeFlag::UV0 | (uint)DefaultVertexInputTypeFlag::NORMAL | (uint)DefaultVertexInputTypeFlag::TANGENT);
}

void RenderManager::DeleteAllDefaultShaders()
{
	for(uint i=0; i<(uint)RenderType::MAX_NUM; ++i)
		DeleteDefaultShader( (RenderType)i );
}

void RenderManager::Destroy()
{
	ClearAllMeshList();
	DeleteAllDefaultShaders();
}

void RenderManager::UpdateRenderList(const Mesh* mesh, bool prevShow)
{
	MeshList::meshkey meshAddress = reinterpret_cast<MeshList::meshkey>(mesh);

	auto GetMeshList = [&](MeshRenderer::Type renderType)
	{
		MeshList* meshList = nullptr;

		if( renderType == MeshRenderer::Type::Transparent )
			meshList = &_transparentMeshes;
		else if( renderType == MeshRenderer::Type::Opaque )
			meshList = &_opaqueMeshes;
		else if( renderType == MeshRenderer::Type::AlphaBlend )
			meshList = &_alphaBlendMeshes;
		else
			ASSERT_MSG("Error, unsupported mesh type");
		
		return meshList;
	};

	MeshRenderer::Type currentType	= mesh->GetMeshRenderer()->GetCurrentRenderType();
	MeshRenderer::Type prevType		= mesh->GetPrevRenderType();

	if( (prevType == currentType) && (prevShow == mesh->GetShow()) )
		return; // not changed

	const std::string& vbKey = mesh->GetMeshFilter()->GetVertexBuffer()->GetKey();

	auto Delete = [&](MeshList* meshList) -> void
	{
		std::set<MeshList::meshkey>* meshSets = meshList->meshes.Find(vbKey);
		if(meshSets)
		{
			meshSets->erase(meshAddress);

			if(meshSets->empty())
				meshList->meshes.Delete(vbKey);

			++meshList->updateCounter;
		}
	};

	if(prevType != MeshRenderer::Type::Unknown)
		Delete(GetMeshList(prevType));

	if(currentType != MeshRenderer::Type::Unknown)
	{
		MeshList* currentMeshList = GetMeshList(currentType);

		if(mesh->GetShow())
		{
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
		}
		else
		{
			Delete(currentMeshList);
		}

		++currentMeshList->updateCounter;
	}
}

bool RenderManager::HasMeshInRenderList(const Mesh* mesh, MeshRenderer::Type type)
{
	MeshList::meshkey meshAddress = reinterpret_cast<MeshList::meshkey>(mesh);
	const Mesh* foundedMesh = nullptr;
	const std::string& vbKey = mesh->GetMeshFilter()->GetVertexBuffer()->GetKey();

	MeshList* meshList = nullptr;

	if(type == MeshRenderer::Type::Transparent)
		meshList = &_transparentMeshes;
	else if(type == MeshRenderer::Type::Opaque)
		meshList = &_opaqueMeshes;
	else if(type == MeshRenderer::Type::AlphaBlend)
		meshList = &_alphaBlendMeshes;
	else
	{
		ASSERT_MSG("Error!, undefined MeshRenderer::Type");
	}

	std::set<MeshList::meshkey>* meshSets = meshList->meshes.Find(vbKey);
	if(meshSets)
	{
		auto foundIter = meshSets->find(meshAddress);
		foundedMesh = (foundIter != meshSets->end()) ? reinterpret_cast<Mesh*>(*foundIter) : nullptr;
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