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
			enum BasicConstBuffercSlot
			{
				Transform = 0,
				MaterialColor
			};

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
				Ambient = 0,
				Diffuse,
				Normal,
				Specular,
				Opacity,
				User
			};



		private:
			Shader::VertexShader*	_vertexShader;
			Shader::PixelShader*	_pixelShader;
			std::string				_name;
			Color					_color;

			std::vector<Shader::BaseShader::BufferType> _constBuffer;				
			Buffer::ConstBuffer*	_colorBuffer;				

			std::vector<Shader::BaseShader::TextureType>	_textures;

		public:
			Material(const std::string& name);
			Material(const std::string& name, const Color& lightColor);
			~Material(void);

		public:
			void InitColorBuffer(ID3D11DeviceContext* context);
			void UpdateColorBuffer(ID3D11DeviceContext* context);

			bool UpdateTexture(unsigned int index, const Texture::Texture* texture);
			void UpdateAmbientMap(const Texture::Texture* tex);
			void UpdateDiffuseMap(const Texture::Texture* tex);
			void UpdateNormalMap(const Texture::Texture* tex);
			void UpdateSpecularMap(const Texture::Texture* tex);
			void UpdateOpacityMap(const Texture::Texture* tex);

		public:
			GET_ACCESSOR(Name, const std::string&, _name);
			GET_ACCESSOR(ColorBuffer, Buffer::ConstBuffer*, _colorBuffer);
			GET_ACCESSOR(ConstBuffers, const std::vector<Shader::BaseShader::BufferType>&, _constBuffer);

			GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
			GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
			GET_SET_ACCESSOR(Textures, const std::vector<Shader::BaseShader::TextureType>&, _textures);
			GET_SET_ACCESSOR(Color, const Color&, _color);
		};

	}
}