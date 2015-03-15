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
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool recycleCode, const std::string* includeCode = nullptr);
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& command, bool recyleCode, const std::string* includeCode = nullptr);

		private:
			bool CommandValidator(const std::string& fullCommand, std::string* outFileName, std::string* outShaderType, std::string* outMainFunc);
			bool CommandValidator(const std::string& partlyCommand, const std::string& shaderType, std::string* outFileName, std::string* outMainFunc);

		public:
			bool LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool recycleCode);
			Shader::VertexShader* LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::string* includeFileName = nullptr);
			
			Shader::PixelShader* LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::string* includeFileName = nullptr);
			bool Add(const std::string& fullCommand, Shader::BaseShader* shader);
			bool LoadShader(const std::string filePath);

		public:
			Shader::BaseShader*		FindShader(const std::string& fileName, const std::string& mainFunc, Shader::BaseShader::Type type);

			Shader::VertexShader*	FindVertexShader(const std::string& fileName, const std::string& mainFunc);

			Shader::PixelShader*	FindPixelShader(const std::string& fileName, const std::string& mainFunc);
			const std::string*		FindShaderCode(const std::string& fileName);

		public:
			void RemoveAllShaderCode();
			void RemoveAllShader();
			void RemoveShaderCode(const std::string& command);
			void RemoveShader(const std::string& command);
		};
	}
}