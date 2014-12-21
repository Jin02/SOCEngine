#pragma once

#include <map>
#include "BaseStructure.h"
#include <functional>

namespace Structure
{
	template <typename Key, class Object>
	class Map : public BaseStructure<Key, Object>
	{
	private:
		std::map<Key, BaseStructure<Key, Object>::Data> _map;

	public:
		Map(void){}
		virtual ~Map(void)
		{
			DeleteAll(true);
		}

	public:
		virtual Object* Add(const Key& key, Object* object, bool copy = false)
		{
			BaseStructure<Key, Object>::Data data;
			data.first = copy;
			data.second = copy ? new Object((*object)) : object;

			_map.insert(std::map<Key, BaseStructure<Key, Object>::Data>::value_type(key, data));

			return object;
		}

		virtual Object* Find(const Key& key)
		{
			std::map<Key, BaseStructure<Key, Object>::Data>::iterator iter = _map.find(key);
			return iter == _map.end() ? nullptr : GET_CONTENT_FROM_ITERATOR(iter);
		}

		virtual void Delete(const Key& key, bool contentRemove = false)
		{
			std::map<Key, BaseStructure<Key, Object>::Data>::iterator iter = _map.find(key);

			if( iter == _map.end() )
				return;

			if(contentRemove)
				SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );

			_map.erase(iter);
		}

		virtual void DeleteAll(bool contentRemove = false)
		{
			if(contentRemove)
			{
				typename std::map<Key, BaseStructure<Key, Object>::Data>::iterator iter;
				for(iter = _map.begin();iter != _map.end(); ++iter)
				{
					if( GET_IS_COPY_FROM_ITERATOR(iter) )
						SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );
				}
			}

			_map.clear();
		}

		void Iterate(const std::function<void(bool isCopy, const Key& key, Object* obj)>& recvFunc) const
		{
			for(auto iter = _map.cbegin(); iter != _map.cend(); ++iter)
				recvFunc(GET_IS_COPY_FROM_ITERATOR(iter), GET_KEY_FROM_ITERATOR(iter), GET_CONTENT_FROM_ITERATOR(iter));
		}
		
		void IterateContent(const std::function<void(Object* obj)>& recvFunc) const
		{
			for(auto iter = _map.cbegin(); iter != _map.cend(); ++iter)
				recvFunc(GET_CONTENT_FROM_ITERATOR(iter));
		}

		inline const std::map<Key, BaseStructure<Key, Object>::Data>& GetMap() const { return _map; }
		GET_ACCESSOR(Size, unsigned int, _map.size());
	};
}