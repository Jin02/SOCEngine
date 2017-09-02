#pragma once

#include "Common.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <type_traits>

namespace Core
{
	template <typename Key, class Map>
	class Bookmark
	{
	public:
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
			auto found = _map.find(key);			
			if (found != _map.end())
				_map.erase(found);
		}

		void DeleteAll()
		{
			_map.clear();
		}

		static constexpr uint FailIndex() { return -1; }//std::numeric_limits<uint>::max(); }

	private:
		Map	_map;
	};

	template<typename Key>
	using BookHashMapmark = Bookmark<Key, std::unordered_map<Key, uint>>;

	template<typename Key>
	using BookMapmark = Bookmark<Key, std::map<Key, uint>>;
}