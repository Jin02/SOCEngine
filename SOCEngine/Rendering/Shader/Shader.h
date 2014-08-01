#pragma once

#include <string>
#include "DirectX.h"

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
			Shader(ID3DBlob* blob) : _blob(blob), _type(Type::Invalid)
			{

			}

			virtual ~Shader(void)
			{
			}

		public:
			virtual void Begin() = 0;
			virtual void End() = 0;

		public:
			GET_ACCESSOR(Shader, ID3DBlob*, _blob);
			GET_ACCESSOR(ShaderType, Type, _type);
		};
	}
}
