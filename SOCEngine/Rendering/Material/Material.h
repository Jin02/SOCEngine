#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "Common.h"
#include "Texture.h"

namespace Rendering
{
	namespace Material
	{
		class Material
		{
		private:
			Shader::VertexShader*					_vertexShader;
			Shader::PixelShader*					_pixelShader;
			std::string								_name;
			Color									_mainColor;
			std::vector<const Texture::Texture*>	_textures;

		public:
			Material(const std::string& name, Shader::VertexShader* vertexShader, Shader::PixelShader* pixelShader);
			~Material(void);

		public:
			bool UpdateTextures(unsigned int index, const Texture::Texture* texture);


		public:
			GET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
			GET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
			GET_ACCESSOR(Name, const std::string&, _name);
			GET_SET_ACCESSOR(MainColor, const Color&, _mainColor);
			GET_SET_ACCESSOR(Textures, const std::vector<const Texture::Texture*>&, _textures);
		};

	}
}