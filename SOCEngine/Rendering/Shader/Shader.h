#pragma once

#include <string>
#include "DirectX.h"
#include "ConstBuffer.h"
#include "Texture.h"
#include <vector>

namespace Rendering
{
	namespace Shader
	{
		typedef std::string ShaderCode;

		class Shader
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
			Shader(ID3DBlob* blob);
			virtual ~Shader(void);

		public:
			typedef std::pair<int, Rendering::Buffer::ConstBuffer*> BufferType;
			typedef std::pair<int, Rendering::Texture::Texture*> TextureType;

		public:
			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
			GET_ACCESSOR(ShaderType, Type, _type);
		};
	}
}
