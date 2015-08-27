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
		private:
			Shader::VertexShader*							_vertexShader;
			Shader::PixelShader*							_pixelShader;
		
			std::vector<Shader::BaseShader::TextureType>	_inputPSTextures;

		public:
			FullScreen();
			virtual ~FullScreen();

		protected:
			void Initialize(const std::string& shaderFileName, const std::string& psName);

		public:
			void Render(const Texture::RenderTexture* outResultRT);
			void Destroy();

		public:
			GET_SET_ACCESSOR(InputPSTextures, const std::vector<Shader::BaseShader::TextureType>&, _inputPSTextures);
		};
	}
}