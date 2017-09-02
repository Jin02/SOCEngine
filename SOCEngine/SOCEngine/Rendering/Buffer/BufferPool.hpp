#pragma once

#include <memory>
#include "VectorIndexer.hpp"
#include "Bookmark.hpp"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer;
	};

	namespace Manager
	{
		template <typename BufferType>
		class BufferPool final
		{
		public:
			BufferPool() = default;
			DISALLOW_ASSIGN_COPY(BufferPool<BufferType>);

			void Add(const std::string& file, const std::string& key, const BufferType& bufferData)
			{
				_buffers.Add(file + ":" + key, bufferData);
			}

			auto Find(const std::string& file, const std::string& key)
			{
				std::string findKey = file + ":" + key;
				return _buffers.Find(findKey);
			}

			bool Has(const std::string& file, const std::string& key) const
			{
				return _buffers.Has(file + ":" + key);
			}

			void DeleteBuffer(const std::string& file, const std::string& key)
			{
				uint findIndex = _buffers.GetIndexer().Find(file + ":" + key);

				if (findIndex != -1)
					_buffers.Delete(findIndex);
			}

			void Destroy()
			{
				_buffers.DeleteAll();
			}

		private:
			Core::VectorMap<std::string, BufferType>		_buffers;
		};

		template <>
		class BufferPool<Buffer::VertexBuffer> final
		{
		public:
			BufferPool() = default;
			DISALLOW_ASSIGN_COPY(BufferPool);

			static uint MakeKey(const std::string& fileName, Buffer::VertexBuffer::Key vbChunkKey)
			{
				return std::hash<std::string>()(fileName + std::to_string(vbChunkKey));
			}
			static std::string MakeStrKey(const std::string& fileName, Buffer::VertexBuffer::Key vbChunkKey)
			{
				return fileName + ":" + std::to_string(vbChunkKey);
			}

			void Add(const std::string& file, Buffer::VertexBuffer::Key vbChunkKey, const Buffer::VertexBuffer& bufferData)
			{
				std::string strKey = MakeStrKey(file, vbChunkKey);

				_idMarker.Add(strKey, bufferData.GetKey());
				_buffers.Add(bufferData.GetKey(), bufferData);
			}

			auto Find(const std::string& file, Buffer::VertexBuffer::Key vbChunkKey)
			{
				std::string strKey = MakeStrKey(file, vbChunkKey);

				uint hashKey = _idMarker.Find(strKey);
				return _buffers.Find(hashKey);
			}
			bool Has(const std::string& file, Buffer::VertexBuffer::Key vbChunkKey) const
			{
				return _idMarker.Has(MakeStrKey(file, vbChunkKey));
			}
			void DeleteBuffer(const std::string& file, Buffer::VertexBuffer::Key vbChunkKey)
			{
				std::string strKey = MakeStrKey(file, vbChunkKey);
				uint findKey = _idMarker.Find(strKey);

				if (findKey != -1)
				{
					_buffers.Delete(findKey);
					_idMarker.Delete(strKey);
				}
			}

			auto Find(Buffer::VertexBuffer::Key vbKey)
			{
				return _buffers.Find(vbKey);
			}
			bool Has(Buffer::VertexBuffer::Key vbKey) const
			{
				return _buffers.Has(vbKey);
			}
			void DeleteBuffer(Buffer::VertexBuffer::Key vbKey)
			{
				auto vb = _buffers.Find(vbKey);
				if (vb == nullptr) return;

				_buffers.Delete(vbKey);
				_idMarker.Delete(vb->GetStrKey());
			}


			void Destroy()
			{
				_buffers.DeleteAll();
				_idMarker.DeleteAll();
			}

		private:
			Core::BookHashMapmark<std::string>			_idMarker;
			Core::VectorMap<uint, Buffer::VertexBuffer> _buffers;
		};

		using VBPool = BufferPool<Buffer::VertexBuffer>;
	}
}