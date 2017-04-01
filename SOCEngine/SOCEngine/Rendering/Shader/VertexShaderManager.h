#pragma once

#include "ShaderPool.hpp"
#include "VertexShader.h"

namespace Rendering
{
	namespace Manager
	{
		class VertexShaderManager final : public ShaderPool<Shader::VertexShader>
		{
		public:
			using ShaderPool<Shader::VertexShader>::ShaderPool;

			//DISALLOW_ASSIGN(VertexShaderManager);

			ShaderPtr LoadShader(Device::DirectX& dx,
				const std::string& folderPath,
				const std::string& fileName,
				const std::string& mainFunc,
				bool useRecycle,
				const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations,
				const std::vector<Shader::ShaderMacro>* macros,
				const std::string* uniqueKey);

		private:
			using ShaderPool<Shader::VertexShader>::LoadShader;
		};
	}
}