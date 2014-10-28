#pragma once

#include <ShaderManager.h>

class ShaderFactory
{
private:
	Rendering::Shader::ShaderManager	*_shaderMgr;

public:
	ShaderFactory(Rendering::Shader::ShaderManager*& shaderManager)
	{
		_shaderMgr = shaderManager;
	}

	~ShaderFactory(void)
	{
	}

public:
	bool LoadShader(const std::string& shaderName,
		const std::string& mainVSFuncName, const std::string& mainPSFuncName,
		Rendering::Shader::VertexShader*& outVertexShader,
		Rendering::Shader::PixelShader*& outPixelShader)
	{
		std::string folderPath = "";
		std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;
		D3D11_INPUT_ELEMENT_DESC desc;
		/** Script Begin **/
		/** Script End **/
		const std::string baseCommand = shaderName+":";
		outVertexShader = dynamic_cast<Rendering::Shader::VertexShader*>(_shaderMgr->LoadVertexShader(folderPath, baseCommand + mainVSFuncName, true, vertexDeclations));
		outPixelShader	= dynamic_cast<Rendering::Shader::PixelShader*>(_shaderMgr->LoadPixelShader(folderPath, baseCommand + mainPSFuncName, false));
		return false;
	}
};

