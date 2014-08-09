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
		private:
			Buffer::VertexBuffer*	_vertexBuffer;
			Buffer::IndexBuffer*	_indexBuffer;

		private:
			bool _alloc;
			unsigned int _numOfVertex;

		public:
			MeshFilter();
			~MeshFilter();

		public:
			bool CreateBuffer(const void* vertexBufferDatas, unsigned int vertexBufferDataCount, unsigned int vertexBufferSize,
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, bool isDynamic);
			void UpdateBuffer(ID3D11DeviceContext* context);
		};
	}
}