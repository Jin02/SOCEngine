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
			
			template <typename BufferType> BufferPool<BufferType>&			GetPool()
			{
				return std::get<BufferPool<BufferType>>(_bufferPools);
			}
			template <typename BufferType> const BufferPool<BufferType>&	GetPool() const
			{
				return std::get<BufferPool<BufferType>>(_bufferPools);
			}

			void Destroy()
			{
				GetPool<Buffer::VertexBuffer>().Destroy();
				GetPool<Buffer::IndexBuffer>().Destroy();
				GetPool<RawBufferType>().Destroy();
			}

		private:
			using RawBufferType = std::shared_ptr<const void>;
			std::tuple<	BufferPool<Buffer::VertexBuffer>,
						BufferPool<Buffer::IndexBuffer>, 
						BufferPool<RawBufferType>>			_bufferPools;
		};
	}
}