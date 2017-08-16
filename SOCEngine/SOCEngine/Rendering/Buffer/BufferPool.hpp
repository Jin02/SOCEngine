#pragma once

#include <memory>
#include "VectorIndexer.hpp"

namespace Rendering
{
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
	}
}