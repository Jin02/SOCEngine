#include "ShaderManager.h"
#include <Shlwapi.h>
#include "Utility.hpp"

#pragma comment(lib, "Shlwapi.lib")

using namespace Rendering::Manager;
using namespace Rendering::Shader;

ShaderManager::ShaderManager() : _shaderCodes()
{
	auto makeKeyFunc = [this](const MakeShaderKeyParam& param) -> std::string
	{
		return MakeKey(param.folderPath + param.fileName, param.mainFunc, param.shaderType, param.macros);
	};

	auto CreateBlobFunc = [this](const MakeShaderKeyParam& param, bool useRecycle) -> DXResource<ID3DBlob>
	{
		return CreateBlob(param.folderPath, param.fileName, param.shaderType, param.mainFunc, useRecycle, param.macros);
	};

	_vsMgr.SetMakeKeyFunc(makeKeyFunc);
	_gsMgr.SetMakeKeyFunc(makeKeyFunc);
	_psMgr.SetMakeKeyFunc(makeKeyFunc);

	_vsMgr.SetCreateBlobFunc(CreateBlobFunc);
	_gsMgr.SetCreateBlobFunc(CreateBlobFunc);
	_psMgr.SetCreateBlobFunc(CreateBlobFunc);
}

void ShaderManager::Compile(
	DXResource<ID3DBlob>* outBlob,
	const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel,
	const std::string& funcName, const std::vector<ShaderMacro>* macros)
{
	(*outBlob) = DXResource<ID3DBlob>(nullptr);
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
				macro.Name = iter.GetName().c_str();
				macro.Definition = iter.GetDefinition().c_str();

				d3dMacros.push_back(macro);
			}
		}

		D3D_SHADER_MACRO nullMacro;
		nullMacro.Name = nullptr;
		nullMacro.Definition = nullptr;

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

	if (outBlob)
		(*outBlob) = DXResource<ID3DBlob>(blob);
}

bool ShaderManager::MakeShaderFileFullPath(std::string& outFullPath, const std::string& folderPath, const std::string& fileName)
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

	if (found == FALSE)
		return false;

	outFullPath = fullPath;
	return true;
}

bool ShaderManager::LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool useRecycle)
{
	// Check preview shader codes.
	{
		const std::string alreadyCode = FindShaderCode(fileName);

		if (alreadyCode.empty() == false)
		{
			outCode = alreadyCode;
			return true;
		}
	}

	std::string filePath = "";
	bool success = MakeShaderFileFullPath(filePath, folderPath, fileName);
	assert(success); // Error, Invalid path

	std::ifstream file(filePath, std::ios::in | std::ios::ate | std::ios::binary);
	assert(file.is_open() && file.good()); // "Error, strange file"

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

	if (useRecycle)
		_shaderCodes.insert(std::make_pair(fileName, outCode));

	return true;
}

DXResource<ID3DBlob> ShaderManager::CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<ShaderMacro>* macros)
{
	std::string fullPath = "";
	bool success = MakeShaderFileFullPath(fullPath, folderPath, fileName);
	if (success == false)
	{
		std::string log = "Error, "; log += fileName + " does not exist.";
		assert(log.c_str() == nullptr);
	}

	std::string code = "";
	assert(LoadShaderCode(code, folderPath, fileName, useRecycle) == false); // Error, cant load shader code

	DXResource<ID3DBlob> blob;
	Compile(&blob, fullPath, code, shaderType + "_5_0", mainFunc, macros);

	return blob;
}

std::string ShaderManager::MakeKey(const std::string& fileName, const std::string& mainFunc, const std::string& shaderTypeStr, const std::vector<ShaderMacro>* macros)
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

void ShaderManager::Destroy()
{
	_shaderCodes.clear();
}

void ShaderManager::DeleteShaderCode(const std::string& key)
{
	auto findIter = _shaderCodes.find(key);

	if (findIter != _shaderCodes.end())
		_shaderCodes.erase(findIter);
}

const char*	ShaderManager::FindShaderCode(const std::string& fileName)
{
	auto findIter = _shaderCodes.find(fileName);
	return (findIter == _shaderCodes.end()) ? "" : findIter->second.c_str();
}