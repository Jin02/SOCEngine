#pragma once

#include <vector>
#include "IndexBook.hpp"

namespace Core
{
	template <typename Key, typename Object, template<typename K> class Indexer>
	class VectorIndexer
	{
	private:
		std::vector<Object>     _vector;
		Indexer<Key>            _map;

	public:
		using IndexerType = Indexer<Key>;
		using VectorType = std::vector<Object>;

		VectorIndexer() : _vector(), _map() {}

		void Add(const Key& key, Object& object)
		{
			_vector.push_back(object);

			uint idx = _vector.size() - 1;
			_map.Add(key, idx);
		}

		void Add(const Key& key, const Object& object)
		{
			Add(key, const_cast<Object&>(object));
		}

		const Object& Get(unsigned int index) const
		{
			return _vector[index];
		}

		Object& Get(unsigned int index)
		{
			return const_cast<Object&>(static_cast<const VectorIndexer<Key, Object, Indexer>*>(this)->Get(index));
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

		GET_ACCESSOR(Size, unsigned int, _vector.size());
	};

	template<typename Key, typename Object>
	class VectorMap : public VectorIndexer<Key, Object, IndexMap> {};
	template<typename Key, typename Object>
	class VectorHashMap : public VectorIndexer<Key, Object, IndexHashMap> {};
}