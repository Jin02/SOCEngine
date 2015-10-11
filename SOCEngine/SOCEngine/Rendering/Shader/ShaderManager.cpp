#include "ShaderManager.h"
#include "Utility.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

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

bool ShaderManager::Compile(ID3DBlob** outBlob, const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel, const std::string& funcName, const std::vector<ShaderMacro>* macros)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	std::vector<D3D_SHADER_MACRO> d3dMacros;
	{
		if(macros)
		{
			for(const auto& iter : (*macros))
			{
				D3D_SHADER_MACRO macro;
				macro.Name			= iter.GetName().c_str();
				macro.Definition	= iter.GetDefinition().c_str();

				d3dMacros.push_back(macro);
			}
		}

		D3D_SHADER_MACRO nullMacro;
		nullMacro.Name = nullptr;
		nullMacro.Definition = nullptr;

		d3dMacros.push_back(nullMacro);
	}

	ID3DBlob* pErrorBlob = nullptr;
	
	HRESULT hr = D3DCompile(
		shaderCode.data(), shaderCode.size(),
		fileFullPath.c_str(), d3dMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, funcName.data(),
		shaderModel.data(), dwShaderFlags, 0,
		outBlob, &pErrorBlob);

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

bool ShaderManager::MakeShaderFileFullPath(std::string& outFullPath, const std::string& folderPath, const std::string& fileName)
{
	bool found = false;
	std::string fullPath = "";

	if( fileName.find(".") == -1)
	{
		const char* format[2] = {".fx", ".hlsl"};
		for(int i=0; i<2; ++i)
		{
			fullPath = folderPath+fileName+format[i];
			found = (bool)PathFileExists(fullPath.c_str());
			if(found)	break;
		}
	}
	else //fileName has format
	{
		fullPath = folderPath+fileName;
		found = (bool)PathFileExists(fullPath.c_str());
	}

	if(found == false)
		return false;

	outFullPath = fullPath;
	return true;
}

bool ShaderManager::LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool useRecycle)
{
	// Check preview shader codes.
	{
		const std::string alreadyCode = FindShaderCode(fileName);

		if(alreadyCode.empty() == false)
		{
			outCode = alreadyCode;
			return true;
		}
	}

	std::string filePath = "";
	bool success = MakeShaderFileFullPath(filePath, folderPath, fileName);
	ASSERT_COND_MSG(success, "Error, Invalid path");

	std::ifstream file(filePath, std::ios::in | std::ios::ate | std::ios::binary);
	ASSERT_COND_MSG(file.is_open() && file.good(), "Error, strange file");

	std::streamoff length = file.tellg();
	file.seekg(0, file.beg);

	char* buffer = new char[(uint)length + 1];
	{
		file.read(buffer, length);
		buffer[length] = '\0';
		outCode = buffer;
	}
	delete buffer;

	file.close();

	if(useRecycle)
		_shaderCodes.insert(std::make_pair(fileName, outCode));

	return true;
}

bool ShaderManager::LoadShaderCode(std::string& outCode, const std::string& fileFullPath, bool useRecycle)
{
	std::string folder, fileName, format;
	if( Utility::String::ParseDirectory(fileFullPath, folder, fileName, format) == false)
		return false;

	return LoadShaderCode(outCode, folder, fileName, useRecycle);
}

ID3DBlob* ShaderManager::CreateBlob(const std::string& fileFullPath, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string code = "";
	bool loadSuccess = LoadShaderCode(code, fileFullPath, useRecycle);
	ASSERT_COND_MSG(loadSuccess, "Error, cant load shader code");

	ID3DBlob* blob = nullptr;
	if( Compile(&blob, fileFullPath, code, shaderType+"_5_0", mainFunc, macros) == false )
	{
		ASSERT_MSG("Shader Compile Error!");
		return nullptr;

	}

	return blob;
}

ID3DBlob* ShaderManager::CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fullPath = "";
	bool success = MakeShaderFileFullPath(fullPath, folderPath, fileName);
	ASSERT_COND_MSG(success, "Error, invalid path");

	return CreateBlob(fullPath, shaderType, mainFunc, useRecycle, macros);
}

ID3DBlob* ShaderManager::CreateBlob(const std::string& folderPath, const std::string& command, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fileName, mainFunc, shaderType;

	if(CommandValidator(command, &fileName, &shaderType, &mainFunc) == false)
	{
		ASSERT_MSG("Command Error");
		return nullptr;
	}

	return CreateBlob(folderPath, fileName, shaderType, mainFunc, useRecycle, macros);
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

VertexShader* ShaderManager::LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::vector<ShaderMacro>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "vs", &fileName, &mainFunc) == false)
	{
		ASSERT_MSG("Command Error");
		return nullptr;
	}

	VertexShader* shader = FindVertexShader(fileName, mainFunc);
	
	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "vs", mainFunc, useRecycle, macros);
		if(blob == nullptr)
			return nullptr;

		shader = new VertexShader(blob);

		bool success = shader->CreateShader(vertexDeclations);		
		ASSERT_COND_MSG(success, "Error, Not Created VS");

		std::string key = VS_FULL_COMMAND(fileName, mainFunc);
		_shaders.insert(std::make_pair(key, shader));
	}

	return shader;
}

PixelShader* ShaderManager::LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "ps", &fileName, &mainFunc) == false)
		return nullptr;

	PixelShader* shader = FindPixelShader(fileName, mainFunc);

	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "ps", mainFunc, useRecycle, macros);
		if(blob == nullptr)
			return nullptr;

		shader = new PixelShader(blob);
		ASSERT_COND_MSG(shader->CreateShader(), "Error, Not Created PS");

		std::string key = PS_FULL_COMMAND(fileName, mainFunc);
		_shaders.insert(std::make_pair(key, shader));
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

ShaderForm* ShaderManager::FindShader(const std::string& fileName, const std::string& mainFunc, ShaderForm::Type type)
{
	if(type == ShaderForm::Type::Vertex)
		return FindVertexShader(fileName, mainFunc);
	else if(type == ShaderForm::Type::Pixel)
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

void ShaderManager::Add(const std::string& fullCommand, Rendering::Shader::ShaderForm* shader)
{
	if(CommandValidator(fullCommand, nullptr, nullptr, nullptr) == false)
		ASSERT_MSG("Error, invalied command");

	_shaders.insert(std::make_pair(fullCommand, shader));
}

const char*	ShaderManager::FindShaderCode(const std::string& fileName)
{
	auto findIter = _shaderCodes.find(fileName);
	return (findIter == _shaderCodes.end()) ? "" : findIter->second.c_str();
}

std::string ShaderManager::MakePartlyCommand(const std::string& shaderName, const std::string& shaderMainFuncName)
{
	return shaderName + ":" + shaderMainFuncName;
}

std::string ShaderManager::MakeFullCommand(const std::string& shaderName, const std::string& shaderMainFuncName, const std::string& shaderType)
{
	return shaderName + ":" + shaderType + ":" + shaderMainFuncName;
}