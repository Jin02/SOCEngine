#include "ComputeShaderManager.h"
#include "EngineShaderFactory.hpp"
#include "Utility.hpp"

using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Rendering::Factory;

ComputeShaderManager::ShaderPtr ComputeShaderManager::LoadShader(
	Device::DirectX& dx,
	const std::string& fileName,
	const std::string& mainFunc,
	const std::vector<ShaderMacro>* macros,
	const std::string* uniqueKey)
{
	EngineShaderFactory factory(nullptr);
	std::string filePath = "";
	factory.FetchShaderFullPath(filePath, fileName);

	assert(filePath.empty() == false);

	MakeShaderKeyParam param;
	{
		std::string format = "";
		Utility::String::ParseDirectory(filePath, param.folderPath, param.fileName, format);
		param.fileName		+= format;
		param.macros		= macros;
		param.mainFunc		= mainFunc;
		param.shaderType	= "cs";
	}

	std::string key = (uniqueKey == nullptr) ? GetMakeKeyFunc()(param) : (*uniqueKey);

	ComputeShaderManager::ShaderPtr shader = nullptr;
	if (Has(key) == false)
	{
		DXResource<ID3DBlob> blob = GetCreateBlobFunc()(param, false);
		shader = std::make_shared<ComputeShader>(blob, key);
		shader->Initialize(dx);

		Add(key, shader);
	}

	return shader;
}