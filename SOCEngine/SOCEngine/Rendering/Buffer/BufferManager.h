#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VectorIndexer.hpp"
#include "BufferPool.hpp"
#include <memory>

namespace Rendering
{
	namespace Manager
	{
		class BufferManager final
		{
		public:
			BufferManager() = default;
			DISALLOW_ASSIGN_COPY(BufferManager);
			void Destroy();

			GET_ACCESSOR(VertexBuffer,	BufferPool<Buffer::VertexBuffer>&,			_vertexBuffers);
			GET_ACCESSOR(IndexBuffer,	BufferPool<Buffer::IndexBuffer>&,			_indexBuffers);
			GET_ACCESSOR(OriginVBDatas,	BufferPool<std::shared_ptr<const void>>&,	_originVertexBufferDatas);

		private:
			BufferPool<Buffer::VertexBuffer>		_vertexBuffers;
			BufferPool<Buffer::IndexBuffer>			_indexBuffers;
			BufferPool<std::shared_ptr<const void>>	_originVertexBufferDatas;
		};
	}
}