#pragma once

#include <string>
#include <fstream>
#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
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
			std::map<std::string, Shader::ShaderForm*>					_shaders;
			std::map<std::string, Shader::ShaderCode>					_shaderCodes;

		public:
			ShaderManager();
			~ShaderManager(void);

		public:
			static bool				Compile(ID3DBlob** outBlob, const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel, const std::string& funcName, const std::vector<Shader::ShaderMacro>* macros);
			static bool				MakeShaderFileFullPath(std::string& outFullPath, const std::string& folderPath, const std::string& fileName);

			static void				MakeKey(std::string& out, const std::string& fullCommand, const std::vector<Shader::ShaderMacro>* macros);
			static void				MakeKey(std::string& out, const std::string& fileName, const std::string& mainFunc, const std::string& shaderTypeStr, const std::vector<Shader::ShaderMacro>* macros);

			ID3DBlob*				CreateBlob(const std::string& fileFullPath, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros);
			ID3DBlob*				CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros);
			ID3DBlob*				CreateBlob(const std::string& folderPath, const std::string& command, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros);

		private:
			bool					CommandValidator(const std::string& fullCommand, std::string* outFileName, std::string* outShaderType, std::string* outMainFunc);
			bool					CommandValidator(const std::string& partlyCommand, const std::string& shaderType, std::string* outFileName, std::string* outMainFunc);
	
		public:
			bool					LoadShaderCode(std::string& outCode, const std::string& fileFullPath, bool useRecycle);
			bool					LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool useRecycle);

			Shader::VertexShader*	LoadVertexShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations, const std::vector<Shader::ShaderMacro>* macros, const std::string* uniqueKey = nullptr);
			Shader::PixelShader*	LoadPixelShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros, const std::string* uniqueKey = nullptr);
			Shader::GeometryShader*	LoadGeometryShader(const std::string& folderPath, const std::string& partlyCommand, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros, const std::string* uniqueKey = nullptr);

			void					Add(const std::string& fullCommand, Shader::ShaderForm* shader);
			bool					LoadShader(const std::string filePath);

			static std::string		MakePartlyCommand(const std::string& shaderName, const std::string& shaderMainFuncName);
			static std::string		MakeFullCommand(const std::string& shaderName, const std::string& shaderMainFuncName, const std::string& shaderType);

		public:
			template<typename ShaderType>
			ShaderType* FindShader(const std::string& key)
			{
				ASSERT_MSG_IF(key.empty() == false, "Error, Key is empty");
				auto findIter = _shaders.find(key);
				return findIter == _shaders.end() ? nullptr : static_cast<ShaderType*>(findIter->second);
			}

			const char*				FindShaderCode(const std::string& fileName);

		public:
			void					DeleteAllShaderCode();
			void					DeleteAllShader();
			void					DeleteShaderCode(const std::string& key);
			void					DeleteShader(const std::string& key);

			void					Destroy();
		};
	}
}