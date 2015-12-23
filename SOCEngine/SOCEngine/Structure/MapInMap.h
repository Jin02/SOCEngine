#pragma once

#include <string>
#include <map>
#include <vector>

namespace Structure
{
	template <typename Key, class Object>
	class MapInMap 
	{
	private:
		std::map<Key, std::map<Key, Object>> _map;

	public:
		MapInMap(void){}
		virtual ~MapInMap(void){}

	public:
		bool Add(const Key& key1, const Key& key2, const Object& object)
		{
			bool success = false;
			const auto& pair = std::make_pair(key2, object);

			std::map<Key, Object>* topMap = nullptr;
			if(Find(&topMap, key1) == false)
			{
				std::map<Key, Object> inMap;
				inMap.insert(pair);

				_map.insert( std::make_pair(key1, inMap) );
				success = true;
			}
			else
			{
				auto findIter = topMap->find(key2);
				if(findIter == topMap->end())
				{
					success = true;
					topMap->insert(pair);
				}
			}

			return success;
		}

		Object* Find(const Key& key1, const Key& key2)
		{
			auto findIter = _map.find(key1);

			if(findIter != _map.end())
			{
				std::map<Key, Object>& inMap = findIter->second;
				auto inFindIter = inMap.find(key2);
				if(inFindIter != inMap.end())
					return &inFindIter->second;
			}

			return nullptr;
		}

		bool Find(std::map<Key, Object>** out, const Key& key1)
		{
			auto findIter = _map.find(key1);

			if(findIter != _map.end() && out)
				(*out) = &findIter->second;

			return findIter != _map.end();
		}

		void Delete(const Key& key1, const Key& key2)
		{
			auto findIter = _map.find(key1);

			if(findIter != _map.end())
			{
				std::map<Key, Object>& inMap = findIter->second;
				auto inFindIter = inMap.find(key2);
				if(inFindIter != inMap.end())
					inMap.erase(inFindIter);
			}
		}

		const Object* Find(const Key& key1, const Key& key2) const
		{
			auto findIter = _map.find(key1);

			if(findIter != _map.end())
			{
				const std::map<Key, Object>& inMap = findIter->second;
				auto inFindIter = inMap.find(key2);
				if(inFindIter != inMap.end())
					return &inFindIter->second;
			}

			return nullptr;
		}

		bool Find(const std::map<Key, Object>** out, const Key& key1) const
		{
			auto findIter = _map.find(key1);

			if(findIter != _map.end() && out)
				(*out) = &findIter->second;

			return findIter != _map.end();
		}

		bool Has(const Key& key1, const Key& key2) const
		{
			return Find(key1, key2) != nullptr;
		}

		bool Has(const Key& key1) const
		{
			return Find(nullptr, key1);
		}

		void Delete(const Key& key1)
		{
			auto findIter = _map.find(key1);
			
			if(findIter != _map.end())
				_map.erase(findIter);
		}

		void DeleteAll()
		{
			_map.clear();
		}

	public:
		inline const std::map<Key, std::map<Key, Object>>& GetMapInMap() const { return _map; }
	};
}