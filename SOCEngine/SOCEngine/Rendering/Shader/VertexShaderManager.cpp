#include "VertexShaderManager.h"

using namespace Rendering::Manager;
using namespace Rendering::Shader;

VertexShaderManager::ShaderPtr VertexShaderManager::LoadShader(Device::DirectX& dx, const std::string& folderPath, const std::string& fileName, const std::string& mainFunc, bool useRecycle, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::vector<ShaderMacro>* macros, const std::string* uniqueKey)
{
	MakeShaderKeyParam param;
	{
		param.fileName		= fileName;
		param.folderPath	= folderPath;
		param.macros		= macros;
		param.mainFunc		= mainFunc;
		param.shaderType	= "vs";
	}

	std::string key = (uniqueKey == nullptr) ? GetMakeKeyFunc()(param) : (*uniqueKey);

	VertexShaderManager::ShaderPtr shader = nullptr;
	if (Has(key) == false)
	{
		DXResource<ID3DBlob> blob = GetCreateBlobFunc()(param, useRecycle);
		shader = std::make_shared<VertexShader>(blob, key);
		shader->Initialize(dx, vertexDeclations);

		Add(key, shader);
	}

	return shader;
}