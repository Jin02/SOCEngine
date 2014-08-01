#pragma once

#include <string>
#include <fstream>
#include "HashMap.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Utility.h"

namespace Rendering
{
	namespace Shader
	{
		class ShaderManager
		{
		private:
			Structure::HashMap<Shader> _shaders;
			Structure::HashMap<ShaderCode> _shaderCodes;

		public:
			ShaderManager()
			{
			}

			~ShaderManager(void)
			{
				RemoveAllShaderCode();
				RemoveAllShader();
			}

		public:
			static bool CompileFromMemory(ID3DBlob** outBlob, const std::string &shaderCode, const std::string& shaderModel, const std::string& funcName)
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

					return false;
				}
				if( pErrorBlob )
					pErrorBlob->Release();

				return true;
			}
			static bool CompileFromFile(ID3DBlob** outBlob, const std::string &fileName, const std::string& shaderModel, const std::string& funcName)
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

					return false;
				}
				if( pErrorBlob ) pErrorBlob->Release();

				return true;
			}

		private:
			bool LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool recycleCode)
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
				const char* extension[2] = {".fx", ".hlsl"};
				for(int i=0; i<2; ++i)
				{
					file.open(folderPath+fileName+extension[i]);

					if(file.is_open())
						break;
				}

				if(file.good() == false)
				{
					file.close();
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
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool recycleCode)
			{
				std::string code;
				if(LoadShaderCode(code, folderPath, fileName, recycleCode) == false)
					return nullptr;

				ID3DBlob* blob = nullptr;
				if( CompileFromMemory(&blob, code, shaderType+"_5_0", mainFunc) == false )
					return nullptr;

				return blob;
			}
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& command, bool recyleCode)
			{
				std::string fileName, mainFunc, shaderType;

				if(CommandValidator(command, &fileName, &shaderType, &mainFunc) == false)
					return nullptr;

				return CreateBlob(folderPath, fileName, shaderType, mainFunc, recyleCode);
			}

			bool CommandValidator(const std::string& fullCommand, std::string* outFileName, std::string* outShaderType, std::string* outMainFunc)
			{
				std::vector<std::string> commands;
				Utility::Tokenize(fullCommand, commands, ":");

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
			bool CommandValidator(const std::string& partlyCommand, const std::string& shaderType, std::string* outFileName, std::string* outMainFunc)			
			{
				std::vector<std::string> commands;
				Utility::Tokenize(partlyCommand, commands, ":");

				if(commands.size() != 2)
					return false;

				if(outFileName)
					(*outFileName) = commands[0];
				if(outMainFunc)
					(*outMainFunc) = commands[1];

				return true;
			}

		public:
			//declation을 직접 입력하는 방식
			Shader* LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode,
				const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count)
			{
				std::string fileName, mainFunc;

				if(CommandValidator(partlyCommand, "vs", &fileName, &mainFunc) == false)
					return nullptr;

				std::string fullCommand = fileName+":vs:"+mainFunc;
				Shader* shader = _shaders.Find(fullCommand);

				if(shader == nullptr)
				{
					ID3DBlob* blob = CreateBlob(folderPath, fileName, "vs", mainFunc, recyleCode);
					if(blob == nullptr)
						return nullptr;

					shader = new VertexShader(blob);
					if(dynamic_cast<VertexShader*>(shader)->CreateShader(vertexDeclations, count))
						_shaders.Add(fullCommand, shader, false);
				}

				return shader;
			}
			Shader* LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode)
			{
				std::string fileName, mainFunc;

				if(CommandValidator(partlyCommand, "ps", &fileName, &mainFunc) == false)
					return nullptr;

				std::string fullCommand = fileName+":ps:"+mainFunc;
				Shader* shader = _shaders.Find(fullCommand);

				if(shader == nullptr)
				{
					ID3DBlob* blob = CreateBlob(folderPath, fileName, "ps", mainFunc, recyleCode);
					if(blob == nullptr)
						return nullptr;

					shader = new PixelShader(blob);
					if(dynamic_cast<PixelShader*>(shader)->CreateShader())
						_shaders.Add(fullCommand, shader, false);
				}

				return shader;
			}

		public:
			void RemoveAllShaderCode()
			{
				_shaderCodes.DeleteAll(true);
			}
			void RemoveAllShader()
			{
				_shaders.DeleteAll(true);
			}
			void RemoveShaderCode(const std::string& command)
			{
				_shaderCodes.Delete(command, true);
			}
			void RemoveShader(const std::string& command)
			{
				_shaders.Delete(command, true);
			}
		};

	}
}