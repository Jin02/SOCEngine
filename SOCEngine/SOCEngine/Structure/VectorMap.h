#pragma once

#include <vector>
#include "BaseStructure.h"
#include <functional>
#include <map>
#include "Utility.h"

namespace Structure
{
	template <typename Key, class Object>
	class VectorMap
	{
	private:
		std::vector<Object>								_vector;

		//pair second value is vector index
		std::map<Key, std::pair<Object*, uint>>			_map;

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

			std::pair<Object*, uint> p;
			p.first = &object;
			p.second = _vector.size() - 1;

			_map.insert(std::make_pair(key, p));
		}

		Object* Find(const Key& key)
		{
			std::map<Key, std::pair<Object*, uint>>::iterator iter = _map.find(key);
			return iter == _map.end() ? nullptr :iter->second.first;
		}

		Object& Get(unsigned int index)
		{
			ASSERT_COND_MSG(index < _vector.size(), "Out index");
			return _vector[index];
		}

		void Delete(const Key& key)
		{
			std::map<Key, std::pair<Object*, uint>>::iterator iter = _map.find(key);
			
			if( iter == _map.end() )
				return;

			_vector.erase(_vector.begin() + iter->second.second);
			_map.erase(iter);
		}

		void DeleteAll()
		{
			_vector.clear();
			_map.clear();
		}

		
		void Iterate(const std::function<void(const Key& key, Object* obj)>& recvFunc) const
		{
			for(auto iter = _map.cbegin(); iter != _map.cend(); ++iter)
				recvFunc(iter.first, iter.second.first);
		}
		
		void IterateContent(const std::function<void(Object* obj)>& recvFunc) const
		{
			for(auto iter = _vector.cbegin(); iter != _vector.cend(); ++iter)
				recvFunc( (*iter) );
		}

		GET_ACCESSOR(Vector, const std::vector<Object>&,		_vector);
		inline const std::map<Key, Object*>& GetMap() const { return _map; }

		GET_ACCESSOR(Size, unsigned int, _vector.size());
	};
}