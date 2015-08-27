#pragma once

#include <vector>
#include <functional>
#include <hash_map>
#include "Utility.h"

#include "Common.h"

namespace Structure
{
	template <typename Key, class Object>
	class VectorHashMap
	{
	protected:
		std::vector<Object>										_vector;

		//pair second value is vector index
		std::hash_map<Key, std::pair<Object*, uint>>			_map;

	public:
		VectorHashMap(void){}
		virtual ~VectorHashMap(void)
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

		Object* Find(const Key& key, uint* outIdx = nullptr)
		{
			std::hash_map<Key, std::pair<Object*, uint>>::iterator iter = _map.find(key);

			Object* ret = nullptr;
			if(iter != _map.end())
			{
				ret = iter->second.first;

				if(outIdx)
					(*outIdx) = iter->second.second;
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
			std::hash_map<Key, std::pair<Object*, uint>>::iterator iter = _map.find(key);
			
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

		GET_ACCESSOR(Vector, const std::vector<Object>&, _vector);
		inline std::hash_map<Key, Object*>& GetHashMap() const { return _map; }

		GET_ACCESSOR(Size, unsigned int, _vector.size());
	};
}