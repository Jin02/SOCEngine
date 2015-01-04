#include "ShaderManager.h"

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

		ASSERT("Shader Compile Error!");

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
		ASSERT("Shader Compile Error!");
		return false;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return true;
}

bool ShaderManager::LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool recycleCode)
{
	{
		std::string* alreadyCode = _shaderCodes.Find(fileName);
		if(alreadyCode)
		{
			outCode = (*alreadyCode);
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
		ASSERT("InValid File");
		return false;
	}

	std::string buff;

	while(std::getline(file, buff))
	{
		outCode += buff;
		outCode += "\n";
	}

	if(recycleCode)
		_shaderCodes.Add(fileName, &outCode, true);

	return true;
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
		ASSERT("Shader Compile Error!");
		return nullptr;

	}
	return blob;
}

ID3DBlob* ShaderManager::CreateBlob(const std::string& folderPath, const std::string& command, bool recyleCode, const std::string* includeCode)
{
	std::string fileName, mainFunc, shaderType;

	if(CommandValidator(command, &fileName, &shaderType, &mainFunc) == false)
	{
		ASSERT("Command Error");
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

VertexShader* ShaderManager::LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::string* includeFileName)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "vs", &fileName, &mainFunc) == false)
	{
		ASSERT("Command Error");
		return nullptr;
	}

	std::string optionalCode;
	if(includeFileName)
		LoadShaderCode(optionalCode, folderPath, (*includeFileName), true);

	std::string fullCommand = VS_FULL_COMMAND(fileName, mainFunc);
	VertexShader* shader = dynamic_cast<VertexShader*>(_shaders.Find(fullCommand));

	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "vs", mainFunc, recyleCode, &optionalCode);
		if(blob == nullptr)
			return nullptr;

		shader = new VertexShader(blob);
		if(shader->CreateShader(vertexDeclations.data(), vertexDeclations.size()))
			_shaders.Add(fullCommand, shader, false);
		else
			ASSERT("Error, Not Created VS");
	}

	return shader;
}

PixelShader* ShaderManager::LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::string* includeFileName)
{
	std::string fileName, mainFunc;

	if(CommandValidator(partlyCommand, "ps", &fileName, &mainFunc) == false)
		return nullptr;

	std::string fullCommand = PS_FULL_COMMAND(fileName, mainFunc);
	PixelShader* shader = dynamic_cast<PixelShader*>(_shaders.Find(fullCommand));

	std::string optionalCode;
	if(includeFileName)
		LoadShaderCode(optionalCode, folderPath, (*includeFileName), true);

	if(shader == nullptr)
	{
		ID3DBlob* blob = CreateBlob(folderPath, fileName, "ps", mainFunc, recyleCode, &optionalCode);
		if(blob == nullptr)
			return nullptr;

		shader = new PixelShader(blob);
		if(shader->CreateShader())
			_shaders.Add(fullCommand, shader, false);
		else
			ASSERT("Error, Not Created PS");
	}

	return shader;
}

void ShaderManager::RemoveAllShaderCode()
{
	_shaderCodes.DeleteAll(true);
}

void ShaderManager::RemoveAllShader()
{
	_shaders.DeleteAll(true);
}

void ShaderManager::RemoveShaderCode(const std::string& command)
{
	_shaderCodes.Delete(command, true);
}

void ShaderManager::RemoveShader(const std::string& command)
{
	_shaders.Delete(command, true);
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
	return dynamic_cast<VertexShader*>(_shaders.Find(VS_FULL_COMMAND(fileName, mainFunc)));
}

PixelShader* ShaderManager::FindPixelShader(const std::string& fileName, const std::string& mainFunc)
{
	return dynamic_cast<PixelShader*>(_shaders.Find(PS_FULL_COMMAND(fileName, mainFunc)));
}

bool ShaderManager::Add(const std::string& fullCommand, Rendering::Shader::BaseShader* shader)
{
	if(CommandValidator(fullCommand, nullptr, nullptr, nullptr) == false)
		ASSERT("Error, invalied command");

	return _shaders.Add(fullCommand, shader, false) ? true : false;
}

const std::string* ShaderManager::FindShaderCode(const std::string& fileName)
{
	return _shaderCodes.Find(fileName);
}