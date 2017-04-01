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
			
			template <typename BufferType>
			BufferPool<BufferType>& GetPool()
			{
				return std::get<BufferPool<BufferType>>(_bufferPools);
			}

		private:
			std::tuple<	BufferPool<Buffer::VertexBuffer>,
						BufferPool<Buffer::IndexBuffer>, 
						BufferPool<std::shared_ptr<const void>> > _bufferPools;
		};
	}
}