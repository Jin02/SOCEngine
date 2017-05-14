#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "IndexBook.hpp"

namespace Core
{
	template <typename Key, typename Object, template<typename K> class Indexer>
	class VectorIndexer
	{
	public:
		using IndexerType = Indexer<Key>;
		using VectorType = std::vector<Object>;

		VectorIndexer() : _vector(), _map() {}

		Object& Add(const Key& key, Object& object)
		{
			_vector.push_back(object);

			uint idx = _vector.size() - 1;
			_map.Add(key, idx);

			return _vector[idx];
		}

		const Object& Add(const Key& key, const Object& object)
		{
			return Add(key, const_cast<Object&>(object));
		}

		const Object& Get(unsigned int index) const
		{
			return _vector[index];
		}

		Object& Get(unsigned int index)
		{
			return const_cast<Object&>(static_cast<const VectorIndexer<Key, Object, Indexer>*>(this)->Get(index));
		}

		const Object* Find(const Key& key) const
		{
			uint findIndex = _map.Find(key);
			bool isFound = findIndex != decltype(_map)::FailIndex();
			return isFound ? &Get(findIndex) : nullptr;
		}

		Object* Find(const Key& key)
		{
			return const_cast<Object*>(static_cast<const VectorIndexer<Key, Object, Indexer>*>(this)->Find(key));
		}

		void Delete(const Key& key)
		{
			uint findIdx = _map.Find(key);
			if (findIdx == Indexer<Key>::FailIndex())
				return;

			uint ereaseIdx = findIdx;
			_vector.erase(_vector.begin() + ereaseIdx);
			_map.Delete(key);
		}

		void DeleteAll()
		{
			_vector.clear();
			_map.DeleteAll();
		}

		inline const std::vector<Object>& GetVector() const { return _vector; }
		inline const Indexer<Key> GetIndexer() const { return _map; }

		GET_CONST_ACCESSOR(Size, unsigned int, _vector.size());

	private:
		std::vector<Object>     _vector;
		Indexer<Key>            _map;
	};

	template<typename Key, typename Object>
	class VectorMap : public VectorIndexer<Key, Object, IndexMap> {};
	template<typename Key, typename Object>
	class VectorHashMap : public VectorIndexer<Key, Object, IndexHashMap> {};
}
