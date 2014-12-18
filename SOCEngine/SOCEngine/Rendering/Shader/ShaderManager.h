#pragma once

#include <string>
#include <fstream>
#include "Map.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Utility.h"

namespace Rendering
{
	namespace Manager
	{
		class ShaderManager
		{
		private:
			Structure::Map<std::string, Shader::BaseShader>		_shaders;
			Structure::Map<std::string, Shader::ShaderCode>		_shaderCodes;

		public:
			ShaderManager();
			~ShaderManager(void);

		public:
			static bool CompileFromMemory(ID3DBlob** outBlob, const std::string &shaderCode, const std::string& shaderModel, const std::string& funcName);
			static bool CompileFromFile(ID3DBlob** outBlob, const std::string &fileName, const std::string& shaderModel, const std::string& funcName);

		private:
			bool LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool recycleCode);
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool recycleCode);
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& command, bool recyleCode);

			bool CommandValidator(const std::string& fullCommand, std::string* outFileName, std::string* outShaderType, std::string* outMainFunc);
			bool CommandValidator(const std::string& partlyCommand, const std::string& shaderType, std::string* outFileName, std::string* outMainFunc);

		public:
			//declation을 직접 입력하는 방식
			Shader::VertexShader* LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations);
			
			Shader::PixelShader* LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode);

		public:
			Shader::BaseShader*		FindShader(const std::string& fileName, const std::string& mainFunc, Shader::BaseShader::Type type);
			Shader::VertexShader*	FindVertexShader(const std::string& fileName, const std::string& mainFunc);
			Shader::PixelShader*	FindPixelShader(const std::string& fileName, const std::string& mainFunc);

		public:
			void RemoveAllShaderCode();
			void RemoveAllShader();
			void RemoveShaderCode(const std::string& command);
			void RemoveShader(const std::string& command);
		};
	}
}