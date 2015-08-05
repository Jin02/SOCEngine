#include "ShaderManager.h"
#include "Utility.h"

using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Utility;

#define VS_FULL_COMMAND(fileName, mainFunc) fileName + ":vs:" + mainFunc
#define PS_FULL_COMMAND(fileName, mainFunc) fileName + ":ps:" + mainFunc

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager(void)
{
	RemoveAllShaderCode();
	RemoveAllShader();
}

bool ShaderManager::CompileFromMemory(ID3DBlob** outBlob, const std::string &shaderCode, const std::string& shaderModel, const std::string& funcName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = nullptr;

	HRESULT hr = D3DX11CompileFromMemory(
		shaderCode.data(), shaderCode.size(),
		nullptr, nullptr, nullptr, funcName.data(),
		shaderModel.data(), dwShaderFlags, 0, nullptr,
		outBlob, &pErrorBlob, nullptr);

	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
		if( pErrorBlob ) pErrorBlob->Release();

		ASSERT_MSG((char*)pErrorBlob->GetBufferPointer() );

		return false;
	}
	if( pErrorBlob )
		pErrorBlob->Release();

	return true;
}

bool ShaderManager::CompileFromFile(ID3DBlob** outBlob, const std::string &fileName, const std::string& shaderModel, const std::string& funcName)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile( fileName.data(), NULL, NULL, funcName.data(), shaderModel.data(), 
		dwShaderFlags, 0, NULL, outBlob, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
		if( pErrorBlob ) pErrorBlob->Release();
		
		ASSERT_MSG("Shader Compile Error!");
		
		return false;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return true;
}

bool ShaderManager::LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool recycleCode)
{
	// Check preview shader codes.
	{
		const std::string& alreadyCode = FindShaderCode(fileName);

		if(alreadyCode.empty() == false)
		{
			outCode = alreadyCode;
			return true;
		}
	}

	std::ifstream file;
	if( fileName.find(".") == -1)
	{
		const char* extension[2] = {".fx", ".hlsl"};
		for(int i=0; i<2; ++i)
		{
			file.open(folderPath+fileName+extension[i]);

			if(file.is_open())
				break;
		}
	}
	else //fileName has extension
	{
		file.open(folderPath+fileName);
	}

	if(file.good() == false)
	{
		file.close();
		ASSERT_MSG("InValid File");
		return false;
	}

	std::string buff;

	while(std::getline(file, buff))
	{
		outCode += buff;
		outCode += "\n";
	}

	if(recycleCode)
		_shaderCodes.insert(std::make_pair(fileName, outCode));

	file.close();

	return true;
}

bool ShaderManager::LoadShaderCode(std::string& outCode, const std::string& fileFullPath, bool recycleCode)
{
	std::string folder, fileName, extension;
	if( Utility::String::ParseDirectory(fileFullPath, folder, fileName, extension) == false)
		return false;

	return LoadShaderCode(outCode, folder, fileName, recycleCode);
}

ID3DBlob* ShaderManager::CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool recycleCode, const std::string* includeCode)
{
	std::string code;
	if(LoadShaderCode(code, folderPath, fileName, recycleCode) == false)
		return nullptr;

	if(includeCode)
		code = (*includeCode) + code;

	ID3DBlob* blob = nullptr;
	if( CompileFromMemory(&blob, code, shaderType+"_5_0", mainFunc) == false )
	{
		ASSERT_MSG("Shader Compile Error!");
		return nullptr;

	}
	return blob;
}

ID3DBlob* ShaderManager::CreateBlob(const std::string& folderPath, const std::string& command, bool recyleCode, const std::string* includeCode)
{
	std::string fileName, mainFunc, shaderType;

	if(CommandValidator(command, &fileName, &shaderType, &mainFunc) == false)
	{
		ASSERT_MSG("Command Error");
		return nullptr;
	}

	return CreateBlob(folderPath, fileName, shaderType, mainFunc, recyleCode, includeCode);
}

bool ShaderManager::CommandValidator(const std::string& fullCommand, std::string* outFileName, std::string* outShaderType, std::string* outMainFunc)
{
	std::vector<std::string> commands;
	String::Tokenize(fullCommand, commands, ":");

	if(commands.size() != 3)
		return false;
	if(outFileName)
		(*outFileName) = commands[0];
	if(outShaderType)
		(*outShaderType) = commands[1];
	if(outMainFunc)
		(*outMainFunc) = commands[2];

	return true;
}

bool ShaderManager::CommandValidator(const std::string& partlyCommand, const std::string& shaderType, std::string* outFileName, std::string* outMainFunc)			
{
	std::vector<std::string> commands;
	String::Tokenize(partlyCommand, commands, ":");

	if(commands.size() != 2)
		return false;
	if(outFileName)
		(*outFileName) = commands[0];
	if(outMainFunc)
		(*outMainFunc) = commands[1];

	return true;
}

VertexShader* ShaderManager::LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::string* includeFileName, const std::vector<std::string>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "vs", &fileName, &mainFunc) == false)
	{
		ASSERT_MSG("Command Error");
		return nullptr;
	}

	std::string optionalCode;
	if(includeFileName)
		LoadShaderCode(optionalCode, folderPath, (*includeFileName), true);
	if(macros)
	{
		for(const auto& iter : (*macros))
			optionalCode.insert(0, iter + "\n");
	}

	VertexShader* shader = FindVertexShader(fileName, mainFunc);
	
	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "vs", mainFunc, recyleCode, &optionalCode);
		if(blob == nullptr)
			return nullptr;

		shader = new VertexShader(blob);

		bool success = shader->CreateShader(vertexDeclations.data(), vertexDeclations.size());		
		ASSERT_COND_MSG(success, "Error, Not Created VS");

		_shaders.insert(std::make_pair(VS_FULL_COMMAND(fileName, mainFunc), shader));
	}

	return shader;
}

PixelShader* ShaderManager::LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::string* includeFileName, const std::vector<std::string>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "ps", &fileName, &mainFunc) == false)
		return nullptr;

	PixelShader* shader = FindPixelShader(fileName, mainFunc);

	std::string optionalCode;

	if(includeFileName)
		LoadShaderCode(optionalCode, folderPath, (*includeFileName), true);

	if(macros)
	{
		for(const auto& iter : (*macros))
			optionalCode.insert(0, iter + "\n");
	}

	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "ps", mainFunc, recyleCode, &optionalCode);
		if(blob == nullptr)
			return nullptr;

		shader = new PixelShader(blob);
		ASSERT_COND_MSG(shader->CreateShader(), "Error, Not Created PS");

		_shaders.insert(std::make_pair(PS_FULL_COMMAND(fileName, mainFunc), shader));
	}

	return shader;
}

void ShaderManager::RemoveAllShaderCode()
{
	_shaderCodes.clear();
}

void ShaderManager::RemoveAllShader()
{
	for(auto iter=_shaders.begin(); iter != _shaders.end(); ++iter)
		SAFE_DELETE(iter->second);

	_shaders.clear();
}

void ShaderManager::RemoveShaderCode(const std::string& command)
{
	auto findIter = _shaderCodes.find(command);

	if(findIter != _shaderCodes.end())
		_shaderCodes.erase(findIter);
}

void ShaderManager::RemoveShader(const std::string& command)
{
	auto findIter = _shaders.find(command);
	if(findIter != _shaders.end())
	{
		SAFE_DELETE(findIter->second);
		_shaders.erase(findIter);
	}
}

BaseShader* ShaderManager::FindShader(const std::string& fileName, const std::string& mainFunc, BaseShader::Type type)
{
	if(type == BaseShader::Type::Vertex)
		return FindVertexShader(fileName, mainFunc);
	else if(type == BaseShader::Type::Pixel)
		return FindPixelShader(fileName, mainFunc);

	return nullptr;
}

VertexShader* ShaderManager::FindVertexShader(const std::string& fileName, const std::string& mainFunc)
{
	auto findIter = _shaders.find(VS_FULL_COMMAND(fileName, mainFunc));
	return findIter == _shaders.end() ? nullptr : dynamic_cast<VertexShader*>(findIter->second);
}

PixelShader* ShaderManager::FindPixelShader(const std::string& fileName, const std::string& mainFunc)
{
	auto findIter = _shaders.find(PS_FULL_COMMAND(fileName, mainFunc));
	return findIter == _shaders.end() ? nullptr : dynamic_cast<PixelShader*>(findIter->second);
}

void ShaderManager::Add(const std::string& fullCommand, Rendering::Shader::BaseShader* shader)
{
	if(CommandValidator(fullCommand, nullptr, nullptr, nullptr) == false)
		ASSERT_MSG("Error, invalied command");

	_shaders.insert(std::make_pair(fullCommand, shader));
}

const std::string& ShaderManager::FindShaderCode(const std::string& fileName)
{
	auto findIter = _shaderCodes.find(fileName);
	return (findIter == _shaderCodes.end()) ? std::string() : findIter->second;
}