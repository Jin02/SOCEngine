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
		public:
			struct Color
			{
				Rendering::Color diffuse;
				Rendering::Color ambient;
				Rendering::Color specular;
				Rendering::Color emissive;

				float shiness;
				float opacity;

				Color();
				~Color();
			};

			enum TextureType
			{
				Diffuse = 0,
				Normal,
				Specular,
				Opacity,
				Other
			};

		private:
			Shader::VertexShader*					_vertexShader;
			Shader::PixelShader*					_pixelShader;
			std::string								_name;
			Color									_color;

			std::vector<const Texture::Texture*>	_textures;

		public:
			Material(const std::string& name);
			Material(const std::string& name, const Color& lightColor);
			~Material(void);

		public:
			bool UpdateTexture(unsigned int index, const Texture::Texture* texture);
			void UpdateDiffuseMap(const Texture::Texture* tex);
			void UpdateNormalMap(const Texture::Texture* tex);
			void UpdateSpecularMap(const Texture::Texture* tex);
			void UpdateOpacityMap(const Texture::Texture* tex);

		public:
			GET_ACCESSOR(Name, const std::string&, _name);
			GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
			GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
			GET_SET_ACCESSOR(Textures, const std::vector<const Texture::Texture*>&, _textures);
			GET_SET_ACCESSOR(Color, const Color&, _color);
		};

	}
}