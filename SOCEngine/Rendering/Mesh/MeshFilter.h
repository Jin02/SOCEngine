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
			MeshFilter() : _vertexBuffer(nullptr), _indexBuffer(nullptr)
			{
				_alloc = false;
				_numOfVertex = 0;
			}

			~MeshFilter()
			{
				if(_alloc == false)
					return;

				SAFE_DELETE(_vertexBuffer);
				SAFE_DELETE(_indexBuffer);
			}

		public:
			bool CreateBuffer(
				const void* vertexBufferDatas,
				unsigned int vertexBufferCount, 
				unsigned int vertexBufferSize,
				const WORD* indices, unsigned int indexCount,
				bool isDynamic)
			{
				if(vertexBufferDatas == nullptr)
					return false;

				_numOfVertex = vertexBufferCount;

				_vertexBuffer = new Buffer::VertexBuffer;
				if( _vertexBuffer->Create(vertexBufferDatas, vertexBufferCount * vertexBufferSize, isDynamic) == false )
					return false;

				_indexBuffer = new Buffer::IndexBuffer;
				if( _indexBuffer->Create(indices, sizeof(WORD) * indexCount) == false )
					return false;

				return true;
			}
		};
	}
}