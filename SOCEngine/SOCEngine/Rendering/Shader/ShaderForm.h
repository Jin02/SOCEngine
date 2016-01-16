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
			enum class Type
			{
				Invalid,
				Vertex,
				Geometry,
				Pixel
			};

		protected:
			ID3DBlob*		_blob;
			Type			_type;

		public:
			ShaderForm(ID3DBlob* blob);
			~ShaderForm(void);

		public:
			struct Usage
			{
				bool useVS, usePS, useGS, useHS;

				Usage();
				Usage(bool useVS, bool useGS, bool useHS, bool usePS);
				~Usage(){}

				void SetUsage(Usage usage);
			};

			struct InputConstBuffer : public Usage
			{
				uint bindIndex;
				const Rendering::Buffer::ConstBuffer* buffer;

				InputConstBuffer();
				InputConstBuffer(uint bindIndex, const Rendering::Buffer::ConstBuffer* buffer);
				InputConstBuffer(uint bindIndex, const Rendering::Buffer::ConstBuffer* buffer, bool useVS, bool useGS, bool useHS, bool usePS);
				InputConstBuffer(uint bindIndex, const Rendering::Buffer::ConstBuffer* buffer, Usage usage);

				~InputConstBuffer(){}
			};

			struct InputTexture : public Usage
			{
				uint bindIndex;
				const Texture::TextureForm* texture;

				InputTexture();
				InputTexture(uint bindIndex, const Texture::TextureForm* texture);
				InputTexture(uint bindIndex, const Texture::TextureForm* texture, bool useVS, bool useGS, bool useHS, bool usePS);
				InputTexture(uint bindIndex, const Texture::TextureForm* texture, Usage usage);

				~InputTexture(){}
			};

			struct InputShaderResourceBuffer : public Usage
			{
				uint bindIndex;
				const Buffer::ShaderResourceBuffer* srBuffer;

				InputShaderResourceBuffer();
				InputShaderResourceBuffer(uint bindIndex, const Buffer::ShaderResourceBuffer* srBuffer);
				InputShaderResourceBuffer(uint bindIndex, const Buffer::ShaderResourceBuffer* srBuffer, bool useVS, bool useGS, bool useHS, bool usePS);
				InputShaderResourceBuffer(uint bindIndex, const Buffer::ShaderResourceBuffer* srBuffer, Usage usage);
				~InputShaderResourceBuffer(){}
			};

			struct InputUnorderedAccessView : public Usage
			{
				uint bindIndex;
				const View::UnorderedAccessView* uav;

				InputUnorderedAccessView();
				InputUnorderedAccessView(uint bindIndex, const View::UnorderedAccessView* uav);
				InputUnorderedAccessView(uint bindIndex, const View::UnorderedAccessView* uav, bool useVS, bool useGS, bool useHS, bool usePS);
				InputUnorderedAccessView(uint bindIndex, const View::UnorderedAccessView* uav, Usage usage);
				~InputUnorderedAccessView(){}
			};


		public:
			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
			GET_ACCESSOR(ShaderType, Type, _type);
		};
	}
}