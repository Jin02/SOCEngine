#pragma once

#include <string>
#include "DirectX.h"
#include "ConstBuffer.h"
#include "Texture2D.h"
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
			//first value is shader slot index
			typedef std::pair<unsigned int, const Rendering::Buffer::BaseBuffer*> BufferType;

			//first value is shader slot index
			typedef std::pair<unsigned int, const Texture::Texture2D*> TextureType;

		public:
			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
			GET_ACCESSOR(ShaderType, Type, _type);
		};
	}
}