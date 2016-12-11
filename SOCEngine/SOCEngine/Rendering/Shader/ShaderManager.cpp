#include "ShaderManager.h"
#include "Utility.h"
#include <Shlwapi.h>
#include "Director.h"

#pragma comment(lib, "Shlwapi.lib")

using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Utility;

#define VS_FULL_COMMAND(fileName, mainFunc) fileName + ":vs:" + mainFunc
#define PS_FULL_COMMAND(fileName, mainFunc) fileName + ":ps:" + mainFunc
#define GS_FULL_COMMAND(fileName, mainFunc) fileName + ":gs:" + mainFunc

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager(void)
{
}

bool ShaderManager::Compile(ID3DBlob** outBlob, const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel, const std::string& funcName, const std::vector<ShaderMacro>* macros)
{
	(*outBlob) = nullptr;
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
		(*outBlob)->Release();

		return false;
	}
	if( pErrorBlob )
		pErrorBlob->Release();

	return true;
}

bool ShaderManager::MakeShaderFileFullPath(std::string& outFullPath, const std::string& folderPath, const std::string& fileName)
{
	BOOL found = FALSE;
	std::string fullPath = "";

	if( fileName.find(".") == -1)
	{
		const char* format[2] = {".fx", ".hlsl"};
		for(int i=0; i<2; ++i)
		{
			fullPath = folderPath+fileName+format[i];
			found = PathFileExists(fullPath.c_str());
			if(found)	break;
		}
	}
	else //fileName has format
	{
		fullPath = folderPath+fileName;
		found = PathFileExists(fullPath.c_str());
	}

	if(found == FALSE)
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
	ASSERT_MSG_IF(success, "Error, Invalid path");

	std::ifstream file(filePath, std::ios::in | std::ios::ate | std::ios::binary);
	ASSERT_MSG_IF(file.is_open() && file.good(), "Error, strange file");

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
	if(LoadShaderCode(code, fileFullPath, useRecycle) == false)
	{
		DEBUG_LOG("Error, cant load shader code");
		return nullptr;
	}

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
	if(success == false)
	{
		std::string log = "Warning, "; log += fileName + " does not exist.";
		DEBUG_LOG(log.c_str());
		return nullptr;
	}

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

void ShaderManager::MakeKey(std::string& out,
							const std::string& fileName, const std::string& mainFunc, const std::string& shaderTypeStr, const std::vector<ShaderMacro>* macros)
{
	std::string fullCommand = fileName + ":" + shaderTypeStr + ":" + mainFunc;
	MakeKey(out, fullCommand, macros);
}

void ShaderManager::MakeKey(std::string& out, const std::string& fullCommand, const std::vector<ShaderMacro>* macros)
{
	std::string key = fullCommand;

	if(macros)
	{
		key += ":";

		std::vector<uint> hashKeys;
		for(auto iter = macros->begin(); iter != macros->end(); ++iter)
		{
			std::hash<std::string> hashString;
			uint hashKey = hashString(iter->GetName() + ":" + iter->GetDefinition());

			hashKeys.push_back(hashKey);
		}

		std::sort(hashKeys.begin(), hashKeys.end());
		for(auto iter = hashKeys.begin(); iter != hashKeys.end(); ++iter)
		{
			char dummy[9] = {0, };
			uint hashCode = (*iter);
			sprintf_s(dummy, "%x", hashCode);

			key += dummy;
		}
	}

	out = key;
}

VertexShader* ShaderManager::LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::vector<ShaderMacro>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "vs", &fileName, &mainFunc) == false)
	{
		ASSERT_MSG("Command Error");
		return nullptr;
	}

	std::string key = "";
	MakeKey(key, VS_FULL_COMMAND(fileName, mainFunc), macros);
	VertexShader* shader = FindShader<VertexShader>(key);
	
	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "vs", mainFunc, useRecycle, macros);
		if(blob == nullptr)
			return nullptr;

		shader = new VertexShader(blob, key);

		const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
		bool success = shader->Create(dx, vertexDeclations);		
		ASSERT_MSG_IF(success, "Error, Not Created VS");

		_shaders.insert(std::make_pair(key, shader));
	}

	return shader;
}

PixelShader* ShaderManager::LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "ps", &fileName, &mainFunc) == false)
		return nullptr;

	std::string key = "";
	MakeKey(key, PS_FULL_COMMAND(fileName, mainFunc), macros);
	PixelShader* shader = FindShader<PixelShader>(key);

	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "ps", mainFunc, useRecycle, macros);
		if(blob == nullptr)
			return nullptr;

		shader = new PixelShader(blob, key);

		const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
		ID3D11Device* device = dx->GetDevice();
		ASSERT_MSG_IF(shader->Create(device), "Error, Not Created PS");

		_shaders.insert(std::make_pair(key, shader));
	}

	return shader;
}

void ShaderManager::Destroy()
{
	DeleteAllShaderCode();
	DeleteAllShader();
}

void ShaderManager::DeleteAllShaderCode()
{
	_shaderCodes.clear();
}

void ShaderManager::DeleteAllShader()
{
	for(auto iter=_shaders.begin(); iter != _shaders.end(); ++iter)
		SAFE_DELETE(iter->second);

	_shaders.clear();
}

void ShaderManager::DeleteShaderCode(const std::string& key)
{
	auto findIter = _shaderCodes.find(key);

	if(findIter != _shaderCodes.end())
		_shaderCodes.erase(findIter);
}

void ShaderManager::DeleteShader(const std::string& key)
{
	auto findIter = _shaders.find(key);
	if(findIter != _shaders.end())
	{
		SAFE_DELETE(findIter->second);
		_shaders.erase(findIter);
	}
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

GeometryShader* ShaderManager::LoadGeometryShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "gs", &fileName, &mainFunc) == false)
		return nullptr;

	std::string key = "";
	MakeKey(key, GS_FULL_COMMAND(fileName, mainFunc), macros);
	GeometryShader* shader = FindShader<GeometryShader>(key);

	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "gs", mainFunc, useRecycle, macros);
		if(blob == nullptr)
			return nullptr;

		shader = new GeometryShader(blob, key);

		const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
		ID3D11Device* device = dx->GetDevice();
		ASSERT_MSG_IF(shader->Create(device), "Error, Can't create GS");

		_shaders.insert(std::make_pair(key, shader));
	}

	return shader;
}