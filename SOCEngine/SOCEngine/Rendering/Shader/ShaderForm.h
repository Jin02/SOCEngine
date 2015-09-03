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

		class ShaderForm
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
			ShaderForm(ID3DBlob* blob);
			virtual ~ShaderForm(void);

		public:
			struct Usage
			{
				bool useVS, usePS, useGS, useHS;
				Usage(bool useVS, bool useGS, bool useHS, bool usePS);
				~Usage(){}

				void SetUsage(Usage usage);
			};

			struct InputConstBuffer : public Usage
			{
				uint semanticIndex;
				const Rendering::Buffer::ConstBuffer* buffer;

				InputConstBuffer(uint semanticIndex, const Rendering::Buffer::ConstBuffer* buffer, bool useVS, bool useGS, bool useHS, bool usePS);
				InputConstBuffer(uint semanticIndex, const Rendering::Buffer::ConstBuffer* buffer, Usage usage);

				~InputConstBuffer(){}
			};

			struct InputTexture : public Usage
			{
				uint semanticIndex;
				const Texture::Texture2D* texture;

				InputTexture(uint semanticIndex, const Texture::Texture2D* texture, bool useVS, bool useGS, bool useHS, bool usePS);
				InputTexture(uint semanticIndex, const Texture::Texture2D* texture, Usage usage);

				~InputTexture(){}
			};

			struct InputShaderResourceBuffer : public Usage
			{
				uint semanticIndex;
				const Buffer::ShaderResourceBuffer* srBuffer;

				InputShaderResourceBuffer(uint semanticIndex, const Buffer::ShaderResourceBuffer* srBuffer, bool useVS, bool useGS, bool useHS, bool usePS);
				InputShaderResourceBuffer(uint semanticIndex, const Buffer::ShaderResourceBuffer* srBuffer, Usage usage);
				~InputShaderResourceBuffer(){}
			};

		public:
			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
			GET_ACCESSOR(ShaderType, Type, _type);
		};
	}
}