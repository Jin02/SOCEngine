#include "ShaderCompiler.h"
#include <Shlwapi.h>
#include <fstream>
#include "DirectX.h"

#include "Utility.hpp"

#pragma comment(lib, "Shlwapi.lib")

using namespace Rendering::Shader;

std::string ShaderCompiler::MakeKey(const std::string& fileName, const std::string& mainFunc, const std::string& shaderTypeStr, const std::vector<ShaderMacro>* macros)
{
	std::string key = fileName + ":" + shaderTypeStr + ":" + mainFunc;

	if (macros)
	{
		key += ":";

		std::vector<uint> hashKeys;
		for (auto iter = macros->begin(); iter != macros->end(); ++iter)
		{
			std::hash<std::string> hashString;
			uint hashKey = hashString(iter->GetName() + ":" + iter->GetDefinition());

			hashKeys.push_back(hashKey);
		}

		std::sort(hashKeys.begin(), hashKeys.end());
		for (auto iter = hashKeys.begin(); iter != hashKeys.end(); ++iter)
		{
			char dummy[9] = { 0, };
			uint hashCode = (*iter);
			sprintf_s(dummy, "%x", hashCode);

			key += dummy;
		}
	}

	return key;
}

DXSharedResource<ID3DBlob> ShaderCompiler::Compile(
	const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel,
	const std::string& funcName, const std::vector<ShaderMacro>* macros)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	std::vector<D3D_SHADER_MACRO> d3dMacros;
	{
		if (macros)
		{
			for (const auto& iter : (*macros))
			{
				D3D_SHADER_MACRO macro;
				macro.Name			= iter.GetName().c_str();
				macro.Definition	= iter.GetDefinition().c_str();

				d3dMacros.push_back(macro);
			}
		}

		D3D_SHADER_MACRO nullMacro;
		nullMacro.Name			= nullptr;
		nullMacro.Definition	= nullptr;

		d3dMacros.push_back(nullMacro);
	}

	ID3DBlob* blob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	HRESULT hr = D3DCompile(
		shaderCode.data(), shaderCode.size(),
		fileFullPath.c_str(), d3dMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, funcName.data(),
		shaderModel.data(), dwShaderFlags, 0,
		&blob, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();

		assert(pErrorBlob->GetBufferPointer() == nullptr);
		blob->Release();
	}
	if (pErrorBlob)
		pErrorBlob->Release();

	return DXSharedResource<ID3DBlob>(blob);
}

std::string ShaderCompiler::MakeShaderFileFullPath(const std::string& folderPath, const std::string& fileName)
{
	BOOL found = FALSE;
	std::string fullPath = "";

	if (fileName.find(".") == -1)
	{
		const char* format[2] = { ".fx", ".hlsl" };
		for (int i = 0; i<2; ++i)
		{
			fullPath = folderPath + fileName + format[i];
			found = PathFileExists(fullPath.c_str());
			if (found)	break;
		}
	}
	else //fileName has format
	{
		fullPath = folderPath + fileName;
		found = PathFileExists(fullPath.c_str());
	}

	return fullPath;
}

std::string ShaderCompiler::LoadShaderCode(const std::string& folderPath, const std::string& fileName, bool useRecycle)
{
	std::string resultCode = "";
	// Check preview shader codes.
	{
		resultCode = FindShaderCode(fileName);

		if (resultCode.empty() == false)
			return resultCode;
	}

	std::string filePath = MakeShaderFileFullPath(folderPath, fileName);
	assert(filePath.empty() == false); // Error, Invalid path

	std::ifstream file(filePath, std::ios::in | std::ios::ate | std::ios::binary);
	assert(file.is_open() && file.good()); // "Error, strange file"

	std::streamoff length = file.tellg();
	file.seekg(0, file.beg);

	char* buffer = new char[(uint)length + 1];
	{
		file.read(buffer, length);
		buffer[length] = '\0';
		resultCode = buffer;
	}
	delete buffer;

	file.close();

	if (useRecycle)
		_shaderCodes.insert(std::make_pair(fileName, resultCode));

	return resultCode;
}

DXSharedResource<ID3DBlob> ShaderCompiler::CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fullPath = MakeShaderFileFullPath(folderPath, fileName);
	if (fullPath.empty())
	{
		std::string log = "Error, "; log += fileName + " does not exist.";
		assert(log.c_str() == nullptr);
	}

	std::string code = LoadShaderCode(folderPath, fileName, useRecycle);
	assert(code.empty() == false); // Error, cant load shader code

	return Compile(fullPath, code, shaderType + "_5_0", mainFunc, macros);
}

void ShaderCompiler::DeleteShaderCode(const std::string& key)
{
	auto findIter = _shaderCodes.find(key);

	if (findIter != _shaderCodes.end())
		_shaderCodes.erase(findIter);
}

std::string	ShaderCompiler::FindShaderCode(const std::string& fileName)
{
	auto findIter = _shaderCodes.find(fileName);
	return (findIter == _shaderCodes.end()) ? "" : findIter->second;
}