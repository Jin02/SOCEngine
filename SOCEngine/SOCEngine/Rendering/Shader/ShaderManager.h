#pragma once

#include <string>
#include <fstream>
#include "VertexShader.h"
#include "PixelShader.h"
#include "Utility.h"
#include "ShaderMacro.h"

#include <map>

namespace Rendering
{
	namespace Manager
	{
		class ShaderManager
		{
		private:
			std::map<std::string, Shader::ShaderForm*>		_shaders;
			std::map<std::string, Shader::ShaderCode>		_shaderCodes;

		public:
			ShaderManager();
			~ShaderManager(void);

		public:
			static bool Compile(ID3DBlob** outBlob, const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel, const std::string& funcName, const std::vector<Shader::ShaderMacro>* macros);
			static bool MakeShaderFileFullPath(std::string& outFullPath, const std::string& folderPath, const std::string& fileName);

			ID3DBlob* CreateBlob(const std::string& fileFullPath, const std::string& shaderType, const std::string& mainFunc, bool recycleCode, const std::vector<Shader::ShaderMacro>* macros);
			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool recycleCode, const std::vector<Shader::ShaderMacro>* macros);

			ID3DBlob* CreateBlob(const std::string& folderPath, const std::string& command, bool recyleCode, const std::vector<Shader::ShaderMacro>* macros);

		private:
			bool CommandValidator(const std::string& fullCommand, std::string* outFileName, std::string* outShaderType, std::string* outMainFunc);
			bool CommandValidator(const std::string& partlyCommand, const std::string& shaderType, std::string* outFileName, std::string* outMainFunc);

		public:
			bool LoadShaderCode(std::string& outCode, const std::string& fileFullPath, bool recycleCode);
			bool LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool recycleCode);

			Shader::VertexShader* LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::vector<Shader::ShaderMacro>* macros);
			Shader::PixelShader* LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool recyleCode, const std::vector<Shader::ShaderMacro>* macros);

			void Add(const std::string& fullCommand, Shader::ShaderForm* shader);
			bool LoadShader(const std::string filePath);

			static std::string MakePartlyCommand(const std::string& shaderName, const std::string& shaderMainFuncName);
			static std::string MakeFullCommand(const std::string& shaderName, const std::string& shaderMainFuncName, const std::string& shaderType);

		public:
			Shader::ShaderForm*		FindShader(const std::string& fileName, const std::string& mainFunc, Shader::ShaderForm::Type type);

			Shader::VertexShader*	FindVertexShader(const std::string& fileName, const std::string& mainFunc);

			Shader::PixelShader*	FindPixelShader(const std::string& fileName, const std::string& mainFunc);
			const char*				FindShaderCode(const std::string& fileName);

		public:
			void RemoveAllShaderCode();
			void RemoveAllShader();
			void RemoveShaderCode(const std::string& command);
			void RemoveShader(const std::string& command);
		};
	}
}