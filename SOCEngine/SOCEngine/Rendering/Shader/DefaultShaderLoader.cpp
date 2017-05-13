#include "DefaultShaderLoader.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Manager;

const ShaderGroup& DefaultShaderLoader::LoadDefaultSahder(
	Device::DirectX& dx, ShaderManager& shaderMgr, Shader::ShaderCompiler& compiler,
	DefaultRenderType renderType, uint defaultVertexInputTypeFlag,
	const std::string* customShaderFileName, const std::vector<ShaderMacro>* macros)
{
	std::string fileName = 
		(customShaderFileName == nullptr)? 
		MakeDefaultSahderFileName(renderType, defaultVertexInputTypeFlag) : 
		(*customShaderFileName);

	// check exist
	{
		auto findIter = _shaders.find(fileName);
		if (findIter != _shaders.end())
			return findIter->second;
	}

	std::vector<ShaderMacro> targetShaderMacros;
	{
		if (macros)
			targetShaderMacros = (*macros);

		if (renderType == DefaultRenderType::GBuffer_AlphaBlend ||
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

	auto LoadShader = [&dx = dx, &shaderMgr = shaderMgr, &compiler = compiler](
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
	{
		std::string vsMainFunc = "", gsMainFunc = "", psMainFunc = "";
		MakeDefaultShaderMainFuncNames(vsMainFunc, gsMainFunc, psMainFunc, renderType);

		shader = LoadShader(fileName, vsMainFunc, psMainFunc, gsMainFunc, &targetShaderMacros);
		if(shader.IsAllEmpty() == false)
			_shaders.insert(std::make_pair(fileName, shader));
	}

	return _shaders.find(fileName)->second;
}

void DefaultShaderLoader::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr, Shader::ShaderCompiler& compiler)
{
	std::vector<ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = dx.GetMSAAShaderMacro();
		macros.push_back(msaaMacro);	
	}

	for(uint i=0; i< ((uint)DefaultRenderType::MAX_NUM); ++i)
	{
		DefaultRenderType type = (DefaultRenderType)i;

		LoadDefaultSahder(dx, shaderMgr, compiler, type,
			static_cast<uint>(DefaultVertexInputTypeFlag::UV0) | 
			static_cast<uint>(DefaultVertexInputTypeFlag::NORMAL), nullptr, &macros);

		LoadDefaultSahder(dx, shaderMgr, compiler, type,
			static_cast<uint>(DefaultVertexInputTypeFlag::UV0) | 
			static_cast<uint>(DefaultVertexInputTypeFlag::NORMAL) | 
			static_cast<uint>(DefaultVertexInputTypeFlag::TANGENT), nullptr, &macros);
	}
}

void DefaultShaderLoader::Destroy()
{
	_shaders.clear();
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

bool DefaultShaderLoader::FindShader(ShaderGroup& out,
	uint bufferFlag, DefaultRenderType renderType) const
{
	std::string key = MakeDefaultSahderFileName(renderType, bufferFlag);

	auto iter = _shaders.find(key);
	if(iter == _shaders.end())
		return false;

	out = iter->second;
	return true;
}

bool DefaultShaderLoader::Has(uint bufferFlag, DefaultRenderType renderType) const
{
	std::string key = MakeDefaultSahderFileName(renderType, bufferFlag);
	return _shaders.find(key) != _shaders.end();
}

bool DefaultShaderLoader::Has(const std::string & fileName) const
{
	return _shaders.find(fileName) != _shaders.end();
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
	case DefaultRenderType::GBuffer_AlphaBlend:
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