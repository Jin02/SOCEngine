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
		std::map<Key, std::map<Key, Object>> _mim;

	public:
		MapInMap(void){}
		virtual ~MapInMap(void){}

	public:
		bool Add(const Key& key1, const Key& key2, const Object& object)
		{
			if(Find(key1, key2))
				return false;

			std::map<Key, Object> inMap;
			inMap.insert(std::make_pair(key2, object));

			_mim.insert(std::make_pair(key1, inMap));
			return true;
		}

		Object* Find(const Key& key1, const Key& key2)
		{
			auto findIter = _mim.find(key1);

			if(findIter != _mim.end())
			{
				std::map<Key, Object>& inMap = findIter->second;
				auto inFindIter = inMap.find(key2);
				if(inFindIter != inMap.end())
					return &inFindIter->second;
			}

			return nullptr;
		}

		bool Find(std::map<Key, Object>& out, const Key& key1)
		{
			auto findIter = _mim.find(key1);

			if(findIter != _mim.end())
				out = findIter->second;

			return findIter != _mim.end();
		}

		void Delete(const Key& key1, const Key& key2)
		{
			auto findIter = _mim.find(key1);

			if(findIter != _mim.end())
			{
				std::map<Key, Object>& inMap = findIter->second;
				auto inFindIter = inMap.find(key2);
				if(inFindIter != inMap.end())
					inMap.erase(inFindIter);
			}
		}

		void Delete(const Key& key1)
		{
			auto findIter = _mim.find(key1);
			
			if(findIter != _mim.end())
				_mim.erase(findIter);
		}

		void DeleteAll()
		{
			_mim.clear();
		}

	public:
		inline const std::map<Key, std::map<Key, Object>>& GetMapInMap() const { return _mim; }
	};
}