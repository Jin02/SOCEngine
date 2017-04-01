#pragma once

#include "ShaderPool.hpp"
#include "ComputeShader.h"

namespace Rendering
{
	namespace Manager
	{
		class ComputeShaderManager final : public ShaderPool<Shader::ComputeShader>
		{
		public:
			using ShaderPool<Shader::ComputeShader>::ShaderPool;

			ShaderPtr LoadShader(Device::DirectX& dx,
				const std::string& fileName,
				const std::string& mainFunc,
				const std::vector<Shader::ShaderMacro>* macros,
				const std::string* uniqueKey);

		private:
			using ShaderPool<Shader::ComputeShader>::LoadShader;
		};
	}
}