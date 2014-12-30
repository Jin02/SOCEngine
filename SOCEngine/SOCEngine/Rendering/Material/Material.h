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
		enum BasicConstBuffercSlot : unsigned int
		{
			Transform		= 0,
			Camera			= 1,
			MaterialColor	= 2,
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

		enum TextureType : unsigned int
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

		bool _hasAlpha;
		bool _changedAlpha;

	public:
		Material(const std::string& name);
		Material(const std::string& name, const Color& lightColor);
		~Material(void);

	public:
		void InitColorBuffer(ID3D11DeviceContext* context);
		void UpdateColorBuffer(ID3D11DeviceContext* context);
		void UpdateBasicConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera);

		void UpdateMap(unsigned int shaderSlotIndex, const Texture::Texture* texture);
		void UpdateAmbientMap(const Texture::Texture* tex);
		void UpdateDiffuseMap(const Texture::Texture* tex);
		void UpdateNormalMap(const Texture::Texture* tex);
		void UpdateSpecularMap(const Texture::Texture* tex);
		void UpdateOpacityMap(const Texture::Texture* tex);

		const Rendering::Texture::Texture* FindMap(unsigned int& outIndex, unsigned int shaderSlotIndex);
		void UpdateColor(const Color& color);

	public:
		void ClearResource(ID3D11DeviceContext* context);
		void UpdateShader(ID3D11DeviceContext* context);
		void UpdateResources(ID3D11DeviceContext* context);

	public:
		GET_ACCESSOR(Name, const std::string&, _name);
		GET_ACCESSOR(ColorBuffer, Buffer::ConstBuffer*, _colorBuffer);

		GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
		GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
		GET_ACCESSOR(Color, const Color&, _color);
		GET_SET_ACCESSOR(UpdateConstBufferMethod, const UpdateCBMethod&, _updateConstBufferMethod);
		GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		GET_SET_ACCESSOR(ChangedAlpha, bool, _changedAlpha);
	};
}