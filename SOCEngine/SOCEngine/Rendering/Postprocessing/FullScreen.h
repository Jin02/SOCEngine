#pragma once

#include <memory>

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
				std::string shaderFileName							= "";
				std::string psName									= "";
				const std::vector<Shader::ShaderMacro>* psMacros	= nullptr;
				bool useViewInfo									= false;
				bool useMSAAMacro									= false;

				InitParam() = default;
				InitParam(std::string _shaderFileName, std::string _psName, decltype(psMacros) _psMacros, bool _useViewInfo = false, bool _useMSAAMacro = false)
					: shaderFileName(_shaderFileName), psName(_psName), psMacros(_psMacros), useViewInfo(_useViewInfo), useMSAAMacro(_useMSAAMacro)
				{

				}
			};

		public:
			void Initialize(Device::DirectX& dx, const InitParam& param, Manager::ShaderManager& shaderManager);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT, bool useOutRTViewportSize = false);

		private:
			std::string								_psUniqueKey = "";
			std::shared_ptr<Shader::VertexShader>	_vs;
			std::shared_ptr<Shader::PixelShader>	_ps;
		};
	}
}