#pragma once

#include "Common.h"
#include <vector>
#include <unordered_map>
#include <map>

namespace Core
{
	template <typename Key, class Map>
	class IndexBook
	{
	private:
		Map	_map;

	public:
		IndexBook(void) {}
		~IndexBook(void) {}

		void Add(const Key& key, uint index)
		{
			_map.insert(std::make_pair(key, index));
		}

		uint Find(const Key& key) const
		{
			const auto& iter = _map.find(key);

			bool found = iter != _map.end();
			return found ? iter->second : -1;//std::numeric_limits<uint>::max();
		}

		bool Has(const Key& key) const
		{
			return _map.find(key) != _map.end();
		}

		void Delete(const Key& key)
		{
			const auto& iter = _map.find(key);

			if (iter == _map.end())
				return;

			uint ereaseIdx = iter->second;
			_map.erase(iter);

			for (auto& mapIter : _map)
			{
				if (mapIter.second < ereaseIdx)
					mapIter.second -= 1;
			}
		}

		void DeleteAll()
		{
			_map.clear();
		}

		static constexpr uint FailIndex() { return -1; }//std::numeric_limits<uint>::max(); }
	};

	template<typename Key>
	using IndexHashMap = IndexBook<Key, std::unordered_map<Key, uint>>;
	template<typename Key>
	using IndexMap = IndexBook<Key, std::map<Key, uint>>;
}