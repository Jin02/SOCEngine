#include "DefaultShaderLoader.h"
#include "EngineShaderFactory.hpp"
#include "Utility.hpp"

using namespace Utility;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Manager;

DefaultShaderLoader::Shaders& 
DefaultShaderLoader::LoadDefaultSahder(
	Device::DirectX& dx, ShaderManager& shaderMgr,
	DefaultRenderType renderType, uint bufferFlag,
	const std::vector<ShaderMacro>* macros)
{
	uint key = MakeKey(bufferFlag, renderType);

	// check exist
	{
		if (Has(key))
			return Find(key);
	}

	std::vector<ShaderMacro> targetShaderMacros;
	{
		if (macros)
			targetShaderMacros = (*macros);

		if (renderType == DefaultRenderType::GBuffer_AlphaTest ||
			renderType == DefaultRenderType::ReflectionProbe_AlphaTestWithDiffuse)
		{
			ShaderMacro alphaTestMacro;
			alphaTestMacro.SetName("ENABLE_ALPHA_TEST");
			targetShaderMacros.push_back(alphaTestMacro);
		}
		if (renderType == DefaultRenderType::Forward_Transparency || renderType == DefaultRenderType::ReflectionProbe_Transparency)
		{
			ShaderMacro useTransparencyMacro;
			useTransparencyMacro.SetName("RENDER_TRANSPARENCY");
			targetShaderMacros.push_back(useTransparencyMacro);
		}
	}

	std::string fileName = MakeDefaultSahderFileName(renderType, bufferFlag);

	auto LoadShader = [&dx = dx, &shaderMgr = shaderMgr](
		const std::string& fileName,
		const std::string& vsMainName, const std::string& psMainName, const std::string& gsMainName,
		const std::vector<ShaderMacro>* macros) -> ShaderGroup
	{
		Factory::EngineShaderFactory shaderLoader(&shaderMgr);

		ShaderGroup shaders;
		shaderLoader.LoadShader(dx, fileName, vsMainName, psMainName, gsMainName, macros, &shaders.vs, &shaders.ps, &shaders.gs);

		//ASSERT_MSG_IF(shaders.vs, "RenderManager Error : can not load shader");
		return shaders;
	};

	ShaderGroup shader;
	std::string vsMainFunc = "", gsMainFunc = "", psMainFunc = "";
	MakeDefaultShaderMainFuncNames(vsMainFunc, gsMainFunc, psMainFunc, renderType);

	shader = LoadShader(fileName, vsMainFunc, psMainFunc, gsMainFunc, &targetShaderMacros);
	assert(shader.IsAllEmpty() == false);

	return Add(key, shader);
}

void DefaultShaderLoader::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr)
{
	std::vector<ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = dx.GetMSAAShaderMacro();
		macros.push_back(msaaMacro);	
	}

	for(uint i=0; i< static_cast<uint>(DefaultRenderType::MAX_NUM); ++i)
	{
		DefaultRenderType type = static_cast<DefaultRenderType>(i);

		LoadDefaultSahder(	dx, shaderMgr, type,
							static_cast<uint>(DefaultVertexInputTypeFlag::UV0) | 
							static_cast<uint>(DefaultVertexInputTypeFlag::NORMAL),
							&macros	);

		LoadDefaultSahder(	dx, shaderMgr, type,
							static_cast<uint>(DefaultVertexInputTypeFlag::UV0)		| 
							static_cast<uint>(DefaultVertexInputTypeFlag::NORMAL)	| 
							static_cast<uint>(DefaultVertexInputTypeFlag::TANGENT),
							&macros	);
	}
}

void DefaultShaderLoader::Destroy()
{
	_shaders.clear();
}

DefaultShaderLoader::Shaders&
	DefaultShaderLoader::Add(uint key, const ShaderGroup& shaders)
{
	_shaders.insert(std::make_pair(key, Shaders(shaders)));
	return _shaders[key];
}

bool DefaultShaderLoader::Has(uint key) const
{
	auto iter = _shaders.find(key);
	return iter != _shaders.end();
}

const DefaultShaderLoader::Shaders& DefaultShaderLoader::Find(uint key) const
{
	const auto iter = _shaders.find(key);
	assert(iter != _shaders.end());

	return iter->second;
}

void DefaultShaderLoader::MakeDefaultShaderMainFuncNames(
	std::string& outVSMain, std::string& outGSMain, std::string& outPSMain,
	DefaultRenderType renderType)
{
	std::string vsMain = "VS";
	std::string psMain = "PS";
	std::string gsMain = "";

	if(renderType == DefaultRenderType::Forward_AlphaTestWithDiffuse)
	{
		vsMain = "OnlyAlpaTestWithDiffuseVS";
		psMain = "OnlyAlpaTestWithDiffusePS";
	}
	else if(renderType == DefaultRenderType::Voxelization)
		gsMain = "GS";
	else if(renderType == DefaultRenderType::Forward_OnlyDepth)
	{
		vsMain = "DepthOnlyVS";
		psMain = "";
	}
	else if(renderType == DefaultRenderType::ReflectionProbe_OnlyFrontFace			||
			renderType == DefaultRenderType::ReflectionProbe_Transparency			||
			renderType == DefaultRenderType::ReflectionProbe_AlphaTestWithDiffuse	||
			renderType == DefaultRenderType::ReflectionProbe_OnlyDepth)
	{
		if(renderType == DefaultRenderType::ReflectionProbe_AlphaTestWithDiffuse)
		{
			vsMain = "OnlyAlpaTestWithDiffuseVS";
			gsMain = "OnlyAlpaTestWithDiffuseGS";
			psMain = "OnlyAlpaTestWithDiffusePS";
		}
		else if(renderType == DefaultRenderType::ReflectionProbe_OnlyDepth)
		{
			vsMain = "DepthOnlyVS";
			gsMain = "DepthOnlyGS";
			psMain = "";
		}
		else
		{
			gsMain = "GS";
		}
	}

	outVSMain	= vsMain;
	outGSMain	= gsMain;
	outPSMain	= psMain;
}

uint DefaultShaderLoader::MakeKey(	uint bufferFlag,
									DefaultRenderType renderType	)
{
	constexpr uint bitOffset = 
		Compute_2_Exp<static_cast<uint>(BufferFlag::MAX)>::value;

	return (static_cast<uint>(renderType) << bitOffset) | bufferFlag;
}

std::string DefaultShaderLoader::MakeDefaultSahderFileName(DefaultRenderType renderType, uint bufferFlag) const
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
	case DefaultRenderType::Forward_AlphaTestWithDiffuse:
	case DefaultRenderType::Forward_OnlyDepth:
	case DefaultRenderType::Forward_Transparency:
		frontFileName = "PhysicallyBased_Forward_";
		break;
	case DefaultRenderType::GBuffer_AlphaTest:
	case DefaultRenderType::GBuffer_Opaque:
		frontFileName = "PhysicallyBased_GBuffer_";
		break;
	case DefaultRenderType::Voxelization:
		frontFileName = "Voxelization_";
		break;
	case DefaultRenderType::ReflectionProbe_AlphaTestWithDiffuse:
	case DefaultRenderType::ReflectionProbe_OnlyDepth:
	case DefaultRenderType::ReflectionProbe_OnlyFrontFace:
	case DefaultRenderType::ReflectionProbe_Transparency:
		frontFileName = "ReflectionProbe_";
		break;
	default:
		assert(!"Error, unsupported mesh type");
		break;
	}

	return frontFileName + defaultVertexInputTypeStr;
}

DefaultShaderLoader::Shaders::Shaders(const ShaderGroup& shaderGroup)
	: vs(*shaderGroup.vs), ps(*shaderGroup.ps), gs(*shaderGroup.gs)
{
}
