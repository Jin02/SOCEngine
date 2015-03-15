#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "VertexShader.h"
#include "PixelShader.h"

#include "EngineMath.h"
#include "Color.h"
#include <vector>

namespace Rendering
{
	namespace Mesh
	{
		typedef unsigned int Count; 

		class MeshFilter
		{
		public:
			enum class BufferElement : unsigned char
			{
				Normal		= 1,
				UV			= 2,
				Tangent		= 4,
				Binormal	= 8
			};
			typedef unsigned char BufferElementFlag;

		private:
			Buffer::VertexBuffer*	_vertexBuffer;
			Buffer::IndexBuffer*	_indexBuffer;

			BufferElementFlag		_flag;

		private:
			bool _alloc;
			unsigned int _numOfVertex;

		public:
			MeshFilter();
			~MeshFilter();

		public:
			bool CreateBuffer(const void* vertexBufferDatas, unsigned int vertexBufferDataCount, unsigned int vertexBufferSize,
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, bool isDynamic, BufferElementFlag flag);
			void IASetBuffer(ID3D11DeviceContext* context);

			GET_ACCESSOR(BufferElementFlag, unsigned int, _flag);
		};
	}
}