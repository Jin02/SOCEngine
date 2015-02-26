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
		//이 값들은 인덱스 값으로도 쓰입니다.
		//Common.hlsl에 정의되어 있는 레지스터 인덱스와 연결되어 있습니다.
		enum class DefaultConstBufferSlot : unsigned int
		{
			Transform		= 0,
			Camera			= 1,
		};

	public:
		enum class UpdateCBMethod	{ Default,	Custom	};
		enum class UsageTextureType	{ Vertex,	Pixel	};

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

		bool	_hasAlpha;
		bool	_changedAlpha;

	public:
		Material(const std::string& name, UpdateCBMethod updateMethd = UpdateCBMethod::Default);
		~Material(void);

	protected:
		void Init(const std::vector<unsigned int>& vsConstBufferUsageIndices, const std::vector<unsigned int >& psConstBufferUsageIndices,
				  const std::vector<unsigned int>& vsTextureUsageIndices, const std::vector<unsigned int>& psTextureUsageIndices);

		void InitConstBufferSlot(const std::vector<unsigned int>& vsConstBufferUsageIndices,
								 const std::vector<unsigned int >& psConstBufferUsageIndices);

		void InitTextureSlot(const std::vector<unsigned int>& vsTextureUsageIndices,
							 const std::vector<unsigned int>& psTextureUsageIndices);

	public:
		void ClearResource(ID3D11DeviceContext* context);
		void UpdateShader(ID3D11DeviceContext* context);
		void UpdateResources(ID3D11DeviceContext* context);

	public:
		const Rendering::Texture::Texture* FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex, UsageTextureType usageType);

		bool UpdateTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture, UsageTextureType usageType);		
		bool UpdateTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture* texture, UsageTextureType usageType);

		void UpdateConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera);

	protected:
		void UpdateDefaultConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera);
		virtual void UpdateCustomConstBuffer(ID3D11DeviceContext* context, const Buffer::ConstBuffer* transform, const Buffer::ConstBuffer* camera){};

	public:
		GET_ACCESSOR(Name, const std::string&, _name);
		GET_ACCESSOR(Textures, const Textures&, _textures);

		GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
		GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
		GET_SET_ACCESSOR(UpdateConstBufferMethod, const UpdateCBMethod&, _updateConstBufferMethod);
		GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		GET_SET_ACCESSOR(ChangedAlpha, bool, _changedAlpha);
	};
}