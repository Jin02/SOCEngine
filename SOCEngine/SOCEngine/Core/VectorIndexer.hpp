#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "Indexer.hpp"

namespace Core
{
	template <typename Key, typename Object, class IndexerClass>
	class VectorIndexer
	{
	public:
		using IndexerType	= IndexerClass;
		using VectorType	= std::vector<Object>;

		VectorIndexer() : _vector(), _indexer() {}

		Object& Add(const Key& key, Object&& object)
		{
			return Add(key, object);
		}

		Object& Add(const Key& key, Object& object)
		{
			uint idx = _vector.size();
			_indexer.Add(key, idx);

			_vector.push_back(object);
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
			return const_cast<Object&>(static_cast<const VectorIndexer<Key, Object, IndexerType>*>(this)->Get(index));
		}

		const Object* Find(const Key& key) const
		{
			uint findIndex = _indexer.Find(key);
			bool isFound = findIndex != decltype(_indexer)::FailIndex();
			return isFound ? &Get(findIndex) : nullptr;
		}

		Object* Find(const Key& key)
		{
			return const_cast<Object*>(static_cast<const VectorIndexer<Key, Object, IndexerType>*>(this)->Find(key));
		}

		void Delete(const Key& key)
		{
			uint findIDx = _indexer.Find(key);
			if (findIDx == IndexerType::FailIndex())
				return;

			uint ereaseIDx = findIDx;
			_vector.erase(_vector.begin() + ereaseIDx);
			_indexer.Delete(key);
		}

		void DeleteAll()
		{
			_vector.clear();
			_indexer.DeleteAll();
		}

		GET_CONST_ACCESSOR(Vector,		const auto&,	_vector);
		GET_CONST_ACCESSOR(Indexer,		const auto&,	_indexer);
		GET_CONST_ACCESSOR(Size,		unsigned int,	_vector.size());

	private:
		std::vector<Object>     _vector;
		IndexerType				_indexer;
	};

	template<typename Key, typename Object>
	using VectorMap = VectorIndexer<Key, Object, IndexMap<Key>>;
	template<typename Key, typename Object>
	using VectorHashMap = VectorIndexer<Key, Object, IndexHashMap<Key>>;
}
