#pragma once

#include <string>
#include "DirectX.h"
#include "ConstBuffer.h"
#include "Texture2D.h"
#include "ShaderResourceBuffer.h"
#include <vector>

#define BASIC_SHADER_NAME "Basic"
#define BASIC_NORMAL_MAPPING_SHADER_NAME "BasicNormalMapping"

#define BASIC_VS_MAIN_FUNC_NAME "VS"
#define BASIC_PS_MAIN_FUNC_NAME "PS"

namespace Rendering
{
	namespace Shader
	{
		typedef std::string ShaderCode;

		class BaseShader
		{
		public:
			enum Type
			{
				Invalid,
				Vertex,
				Pixel
			};

		protected:
			ID3DBlob*		_blob;
			Type			_type;

		public:
			BaseShader(ID3DBlob* blob);
			virtual ~BaseShader(void);

		public:
			struct Usage
			{
				bool useVS, usePS, useGS, useHS;
				Usage(bool useVS, bool useGS, bool useHS, bool usePS);
				~Usage(){}

				void SetUsage(Usage usage);
			};

			struct BufferType : public Usage
			{
				uint semanticIndex;
				const Rendering::Buffer::BaseBuffer* buffer;

				BufferType(uint semanticIndex, const Rendering::Buffer::BaseBuffer* buffer, bool useVS, bool useGS, bool useHS, bool usePS);
				BufferType(uint semanticIndex, const Rendering::Buffer::BaseBuffer* buffer, Usage usage);

				~BufferType(){}
			};

			struct TextureType : public Usage
			{
				uint semanticIndex;
				const Texture::Texture2D* texture;

				TextureType(uint semanticIndex, const Texture::Texture2D* texture, bool useVS, bool useGS, bool useHS, bool usePS);
				TextureType(uint semanticIndex, const Texture::Texture2D* texture, Usage usage);

				~TextureType(){}
			};

			struct ShaderResourceType : public Usage
			{
				uint semanticIndex;
				const Buffer::ShaderResourceBuffer* srBuffer;

				ShaderResourceType(uint semanticIndex, const Buffer::ShaderResourceBuffer* srBuffer, bool useVS, bool useGS, bool useHS, bool usePS);
				ShaderResourceType(uint semanticIndex, const Buffer::ShaderResourceBuffer* srBuffer, Usage usage);
				~ShaderResourceType(){}
			};

		public:
			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
			GET_ACCESSOR(ShaderType, Type, _type);
		};
	}
}