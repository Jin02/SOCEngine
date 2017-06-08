#pragma once

#include <string>
#include <tuple>
#include <map>
#include <vector>

#include "ShaderMacro.hpp"
#include "DXResource.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "ComputeShader.h"

namespace Rendering
{
	namespace Shader
	{
		class ShaderCompiler final
		{
		public:
			ShaderCompiler() = default;

			static std::string			MakeKey(
				const std::string& fileName,
				const std::string& mainFunc, const std::string& shaderTypeStr,
				const std::vector<Shader::ShaderMacro>* macros);

			static DXSharedResource<ID3DBlob>	Compile(const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel, const std::string& funcName, const std::vector<Shader::ShaderMacro>* macros);
			static std::string			MakeShaderFileFullPath(const std::string& folderPath, const std::string& fileName);

			DXSharedResource<ID3DBlob>		CreateBlob(const std::string& folderPath,
				const std::string& fileName, const std::string& shaderType,
				const std::string& mainFunc, bool useRecycle, 
				const std::vector<Shader::ShaderMacro>* macros);
		
			std::string					LoadShaderCode(const std::string& folderPath, const std::string& fileName, bool useRecycle);
			std::string					FindShaderCode(const std::string& fileName);
			void						DeleteShaderCode(const std::string& key);

		private:
			std::map<std::string, std::string>	_shaderCodes;
		};
	}
}