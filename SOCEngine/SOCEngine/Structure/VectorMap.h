#pragma once

#include <vector>
#include <functional>
#include <map>
#include "Utility.h"

#include "Common.h"

namespace Structure
{
	template <typename Key, class Object>
	class VectorMap
	{
	protected:
		std::vector<Object>		_vector;
		std::map<Key, uint>		_map;

	public:
		VectorMap(void){}
		virtual ~VectorMap(void)
		{
			DeleteAll();
		}

	public:
		void Add(const Key& key, Object& object)
		{
			_vector.push_back(object);

			uint idx = _vector.size() - 1;
			_map.insert(std::make_pair(key, idx));
		}
		Object* Find(const Key& key, uint* outIdx = nullptr)
		{
			std::map<Key, uint>::iterator iter = _map.find(key);

			Object* ret = nullptr;
			if(iter != _map.end())
			{
				ret = &_vector[iter->second];

				if(outIdx)
					(*outIdx) = iter->second;
			}

			return ret;
		}
		Object& Get(unsigned int index)
		{
			ASSERT_COND_MSG(index < _vector.size(), "Out index");
			return _vector[index];
		}
		void Delete(const Key& key)
		{
			std::map<Key, uint>::iterator iter = _map.find(key);

			if( iter == _map.end() )		
				return;

			uint ereaseIdx = iter->second;
			_vector.erase(_vector.begin() + ereaseIdx);
			_map.erase(iter);

			for(auto mapIter = _map.begin(); mapIter != _map.end(); ++mapIter)
			{
				if(mapIter->second < ereaseIdx)
					mapIter->second -= 1;
			}
		}
		void DeleteAll()
		{
			_vector.clear();
			_map.clear();
		}

		inline const std::vector<Object>& GetVector() const { return _vector; }
		inline const std::map<Key, Object*>& GetMap() const { return _map; }

		GET_ACCESSOR(Size, unsigned int, _vector.size());
	};
}