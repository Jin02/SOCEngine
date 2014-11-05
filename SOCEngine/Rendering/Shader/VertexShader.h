#pragma once

#include "BaseShader.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace Shader
	{
		class VertexShader : public BaseShader
		{
		public:
			enum Flag
			{
				POSITION	= 0x01,			//2^0
				NORMAL		= 0x02,			//2^1
				BINORMAL	= 0x04,			//2^2
				TANGENT		= 0x08,			//2^3
				TEXCOORD0	= 0x10,			//2^4
				TEXCOORD1	= 0x20,			//2^5
				TEXCOORD2	= 0x40,			//2^6
				TEXCOORD3	= 0x80,			//2^7
				TEXCOORD4	= 0x100,		//2^8
				TEXCOORD5	= 0x200,		//2^9
				TEXCOORD6	= 0x400,		//2^10
				TEXCOORD7	= 0x800,		//2^11
				TEXCOORD8	= 0x1000,		//2^12
				TEXCOORD9	= 0x2000,		//2^13
				//INSTANCE	= 0x4000,		//2^14
				USER		= 0x2000000		//2^25
			};

		private:
			ID3D11VertexShader* _shader;
			ID3D11InputLayout*	_layout;
			unsigned int		_flags;

		public:
			GET_ACCESSOR(Flag, unsigned int, _flags);

		public:
			VertexShader(ID3DBlob* blob);
			~VertexShader(void);

		public:
			bool CreateShader(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count, unsigned int flag);
			void UpdateShader(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<const Texture::Texture*>* textures);
		};
	}
}
