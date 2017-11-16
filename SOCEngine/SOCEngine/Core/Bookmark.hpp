#pragma once

#include "Common.h"
#include <unordered_map>
#include <map>
#include <limits>

#undef max

namespace Core
{
	template <typename Key, class Map>
	class Bookmark
	{
	public:
		void Add(const Key& key, typename Map::mapped_type markValue)
		{
			assert(Has(key) == false);
			_map.insert(std::make_pair(key, markValue));
		}

		typename Map::mapped_type Find(const Key& key) const
		{
			const auto& iter = _map.find(key);

			bool found = iter != _map.end();
			return found ? iter->second : Fail();
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

		static constexpr typename Map::mapped_type Fail() { return std::numeric_limits<typename Map::mapped_type>::max(); }

	private:
		Map	_map;
	};

	template<typename Key, typename MarkValueType = uint>
	using BookHashMapmark = Bookmark<Key, std::unordered_map<Key, MarkValueType>>;

	template<typename Key, typename MarkValueType = uint>
	using BookMapmark = Bookmark<Key, std::map<Key, MarkValueType>>;
}
