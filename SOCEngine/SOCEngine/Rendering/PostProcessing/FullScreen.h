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
			Shader::VertexShader*							_vertexShader;
			Shader::PixelShader*							_pixelShader;
		
			std::vector<Shader::ShaderForm::InputTexture>	_inputPSTextures;

		public:
			FullScreen();
			virtual ~FullScreen();

		protected:
			void Initialize(const std::string& shaderFileName, const std::string& psName, const std::vector<Shader::ShaderMacro>* macros);

		public:
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT);
			void Destroy();

		public:
			GET_SET_ACCESSOR(InputPSTextures, const std::vector<Shader::ShaderForm::InputTexture>&, _inputPSTextures);
		};
	}
}