#pragma once

#include <memory>
#include "VectorIndexer.hpp"
#include "Bookmark.hpp"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer;
		class IndexBuffer;
	};

	namespace Manager
	{
		template <typename BufferType>
		class BufferPool final
		{
		public:
			BufferPool() = default;
			DISALLOW_ASSIGN_COPY(BufferPool);

			void Add(uint hashKey, const BufferType& bufferData)
			{
				_buffers.Add(hashKey, bufferData);
			}
			auto Find(uint hashKey)
			{
				return _buffers.Find(hashKey);
			}
			bool Has(uint hashKey) const
			{
				return _buffers.Has(hashKey);
			}
			void Delete(uint hashKey)
			{
				auto vb = _buffers.Find(hashKey);
				if (vb == nullptr) return;

				_buffers.Delete(hashKey);
				_idMarker.Delete(vb->GetStrKey());
			}
			void Destroy()
			{
				_buffers.DeleteAll();
			}

		private:
			Core::VectorMap<uint, BufferType>	_buffers;
		};

		using VBPool = BufferPool<Buffer::VertexBuffer>;
		using IBPool = BufferPool<Buffer::IndexBuffer>;
	}
}