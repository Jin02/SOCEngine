#pragma once

#include "Common.h"
#include "DefaultRenderTypes.h"
#include "Shaders.h"
#include "ShaderManager.h"
#include <string>
#include <unordered_map>

namespace Rendering
{
	namespace Manager
	{
		class DefaultShaderLoader
		{
		public:
			void Initialize(Device::DirectX& dx, ShaderManager& shaderMgr);
			void Destroy();
			bool Has(uint bufferFlag, DefaultRenderType renderType) const;
			bool Has(const std::string& fileName) const;
			bool FindShader(Shader::ShaderGroup& out, uint bufferFlag, DefaultRenderType renderType) const;

			std::string MakeDefaultSahderFileName(DefaultRenderType renderType, uint bufferFlag) const;

		private:
			const Shader::ShaderGroup& LoadDefaultSahder(Device::DirectX& dx, ShaderManager& shaderMgr,
				DefaultRenderType renderType, uint defaultVertexInputTypeFlag,
				const std::string* customShaderFileName, const std::vector<Shader::ShaderMacro>* macros);
			void MakeDefaultShaderMainFuncNames(std::string& outVSMain, std::string& outGSMain, std::string& outPSMain, DefaultRenderType renderType);

		private:
			std::unordered_map<std::string, Shader::ShaderGroup> _shaders;
		};
	}
}