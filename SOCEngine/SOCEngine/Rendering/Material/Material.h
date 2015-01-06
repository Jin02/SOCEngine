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

		enum TextureType : unsigned int
		{
			Ambient = 0,
			Diffuse,
			Normal,
			Specular,
			Opacity,
			User
		};

	protected:
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

	protected:
		Shader::VertexShader*	_vertexShader;
		Shader::PixelShader*	_pixelShader;
		std::string				_name;
		ConstBuffers			_constbuffers;
		Textures				_textures;
		UpdateCBMethod			_updateConstBufferMethod;

		bool _hasAlpha;
		bool _changedAlpha;

	public:
		Material(const std::string& name);
		~Material(void);

	public:
		void UpdateBasicConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera);

		void UpdateMap(unsigned int shaderSlotIndex, const Texture::Texture* texture);
		void UpdateAmbientMap(const Texture::Texture* tex);
		void UpdateDiffuseMap(const Texture::Texture* tex);
		void UpdateNormalMap(const Texture::Texture* tex);
		void UpdateSpecularMap(const Texture::Texture* tex);
		void UpdateOpacityMap(const Texture::Texture* tex);

		const Rendering::Texture::Texture* FindMap(unsigned int& outIndex, unsigned int shaderSlotIndex);

	public:
		void ClearResource(ID3D11DeviceContext* context);
		void UpdateShader(ID3D11DeviceContext* context);
		void UpdateResources(ID3D11DeviceContext* context);

	public:
		GET_ACCESSOR(Name, const std::string&, _name);
		GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
		GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
		GET_SET_ACCESSOR(UpdateConstBufferMethod, const UpdateCBMethod&, _updateConstBufferMethod);
		GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		GET_SET_ACCESSOR(ChangedAlpha, bool, _changedAlpha);
	};
}