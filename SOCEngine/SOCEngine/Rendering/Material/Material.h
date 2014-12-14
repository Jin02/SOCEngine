#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "Common.h"
#include "Texture.h"

namespace Rendering
{
	class Material
	{
	public:
		enum BasicConstBuffercSlot
		{
			Transform = 0,
			MaterialColor
		};

		enum UpdateCBMethod
		{
			Default,
			Custom
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
		struct ConstBuffers
		{
			std::vector<Shader::BaseShader::BufferType> usageVS;
			std::vector<Shader::BaseShader::BufferType> usagePS;
		};
		struct Textures
		{
			std::vector<Shader::BaseShader::TextureType> usageVS;
			std::vector<Shader::BaseShader::TextureType> usagePS;
		};

	private:
		Shader::VertexShader*	_vertexShader;
		Shader::PixelShader*	_pixelShader;
		std::string				_name;
		Color					_color;

		Buffer::ConstBuffer*	_colorBuffer;				
		ConstBuffers			_constbuffers;
		Textures				_textures;
		UpdateCBMethod			_updateConstBufferMethod;

	public:
		Material(const std::string& name);
		Material(const std::string& name, const Color& lightColor);
		~Material(void);

	public:
		void InitColorBuffer(ID3D11DeviceContext* context);
		void UpdateColorBuffer(ID3D11DeviceContext* context);
		void UpdateTransformBuffer(ID3D11DeviceContext* context, Buffer::ConstBuffer* transform);

		bool UpdateTexture(unsigned int index, const Texture::Texture* texture);
		void UpdateAmbientMap(const Texture::Texture* tex);
		void UpdateDiffuseMap(const Texture::Texture* tex);
		void UpdateNormalMap(const Texture::Texture* tex);
		void UpdateSpecularMap(const Texture::Texture* tex);
		void UpdateOpacityMap(const Texture::Texture* tex);

	public:
		void ClearResource(ID3D11DeviceContext* context);
		void UpdateShader(ID3D11DeviceContext* context, const std::vector<Shader::PixelShader::SamplerType>& samplers);

	public:
		GET_ACCESSOR(Name, const std::string&, _name);
		GET_ACCESSOR(ColorBuffer, Buffer::ConstBuffer*, _colorBuffer);

		GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
		GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
		GET_SET_ACCESSOR(Color, const Color&, _color);
		GET_SET_ACCESSOR(UpdateConstBufferMethod, const UpdateCBMethod&, _updateConstBufferMethod);
	};
}