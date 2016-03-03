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
			Shader::VertexShader*										_vertexShader;
			Shader::PixelShader*										_pixelShader;
		
			std::vector<Shader::ShaderForm::InputTexture>				_inputTextures;
			std::vector<Shader::ShaderForm::InputConstBuffer>			_inputConstBuffers;
			std::vector<Shader::ShaderForm::InputShaderResourceBuffer>	_inputSRBuffers;

		public:
			FullScreen();
			virtual ~FullScreen();

		protected:
			void Initialize(const std::string& shaderFileName, const std::string& psName, const std::vector<Shader::ShaderMacro>* macros);

		public:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT);
			void Destroy();

		public:
			GET_SET_ACCESSOR(InputTextures,		const std::vector<Shader::ShaderForm::InputTexture>&,				_inputTextures);
			GET_SET_ACCESSOR(InputConstBuffers,	const std::vector<Shader::ShaderForm::InputConstBuffer>&,			_inputConstBuffers);
			GET_SET_ACCESSOR(InputSRBuffers,	const std::vector<Shader::ShaderForm::InputShaderResourceBuffer>&,	_inputSRBuffers);
		};
	}
}