#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "RenderTexture.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class FullScreen
		{
		protected:
			std::string													_vsUniqueKey;
			Shader::VertexShader*										_vertexShader;
			Shader::PixelShader*										_pixelShader;
		
			std::vector<Shader::ShaderForm::InputTexture>				_inputTextures;
			std::vector<Shader::ShaderForm::InputConstBuffer>			_inputConstBuffers;
			std::vector<Shader::ShaderForm::InputShaderResourceBuffer>	_inputSRBuffers;

		public:
			FullScreen();
			virtual ~FullScreen();

		public:
			void Initialize(const std::string& shaderFileName, const std::string& psName, bool useUniqueKey, const std::vector<Shader::ShaderMacro>* macros);

		public:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT, bool useOutRTViewportSize = false);
			void Destroy();

		public:
			GET_SET_ACCESSOR(InputTextures,		const std::vector<Shader::ShaderForm::InputTexture>&,				_inputTextures);
			GET_SET_ACCESSOR(InputConstBuffers,	const std::vector<Shader::ShaderForm::InputConstBuffer>&,			_inputConstBuffers);
			GET_SET_ACCESSOR(InputSRBuffers,	const std::vector<Shader::ShaderForm::InputShaderResourceBuffer>&,	_inputSRBuffers);
		};
	}
}