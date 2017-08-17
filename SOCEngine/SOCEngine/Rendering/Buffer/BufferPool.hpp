#pragma once

#include <memory>
#include "VectorIndexer.hpp"

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

				auto failIDx = Core::VectorMap<std::string, BufferType>::IndexyerType::FailIndex();
				if (findIndex != failIDx)
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

			static uint MakeKey(const std::string& fileName, uint vbChunkKey)
			{
				return std::hash<std::string>()(fileName + std::to_string(vbChunkKey));
			}
			static std::string MakeStrKey(const std::string& fileName, uint vbChunkKey)
			{
				return fileName + ":" + std::to_string(vbChunkKey);
			}

			void Add(const std::string& file, uint vbChunkKey, const Buffer::VertexBuffer& bufferData)
			{
				std::string strKey = MakeStrKey(file, vbChunkKey);

				_indexer.Add(strKey, bufferData.GetKey());
				_buffers.Add(bufferData.GetKey(), bufferData);
			}

			auto Find(const std::string& file, uint vbChunkKey)
			{
				std::string strKey = MakeStrKey(file, vbChunkKey);

				uint hashKey = _indexer.Find(strKey);
				return _buffers.Find(hashKey);
			}
			bool Has(const std::string& file, uint vbChunkKey) const
			{
				return _indexer.Has(MakeStrKey(file, vbChunkKey));
			}
			void DeleteBuffer(const std::string& file, uint vbChunkKey)
			{
				std::string strKey = MakeStrKey(file, vbChunkKey);
				uint findKey = _indexer.Find(strKey);

				if (findKey != -1)
				{
					_buffers.Delete(findKey);
					_indexer.Delete(strKey);
				}
			}

			auto Find(uint vbKey)
			{
				return _buffers.Find(vbKey);
			}
			bool Has(uint vbKey) const
			{
				return _buffers.Has(vbKey);
			}
			void DeleteBuffer(uint vbKey)
			{
				auto vb = _buffers.Find(vbKey);
				if (vb == nullptr) return;

				_buffers.Delete(vbKey);
				_indexer.Delete(vb->GetStrKey());
			}


			void Destroy()
			{
				_buffers.DeleteAll();
				_indexer.DeleteAll();
			}

		private:
			Core::IndexHashMap<std::string>				_indexer;
			Core::VectorMap<uint, Buffer::VertexBuffer> _buffers;
		};

		using VBPool = BufferPool<Buffer::VertexBuffer>;
	}
}