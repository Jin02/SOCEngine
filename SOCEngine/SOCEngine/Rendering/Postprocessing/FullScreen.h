#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "RenderTexture.h"
#include "ShaderManager.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class FullScreen final
		{
		public:
			struct InitParam
			{
				std::string shaderFileName;
				std::string psName;
				const std::vector<Shader::ShaderMacro>* macros;

				InitParam() = default;
				InitParam(const std::string& fileName, const std::string& psFuncName, const std::vector<Shader::ShaderMacro>* macro)
					: shaderFileName(fileName), psName(psFuncName), macros(macro)
				{				
				}
			};

		public:
			void Initialize(Device::DirectX& dx, const InitParam& param, Manager::ShaderManager& shaderManager);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT, bool useOutRTViewportSize = false);

			GET_CONST_ACCESSOR(VSUniqueKey, const auto&, _vsUniqueKey);

		private:
			std::string				_vsUniqueKey = "";
			Shader::VertexShader	_vs;
			Shader::PixelShader		_ps;
		};
	}
}