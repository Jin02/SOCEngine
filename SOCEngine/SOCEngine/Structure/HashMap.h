#pragma once

#include "BaseStructure.h"
#include <hash_map>

namespace Structure
{
	template <typename Key, class Object>
	class HashMap : public BaseStructure<Key, Object>
	{
	private:
		std::hash_map<Key, BaseStructure<Key, Object>::Data> _hash;

	public:
		HashMap(void){}
		virtual ~HashMap(void)
		{
			DeleteAll(true);
		}

	public:
		virtual Object* Add(const Key& key, Object* object, bool copy = false)
		{
			BaseStructure<Key, Object>::Data data;
			data.first = copy;
			data.second = copy ? new Object((*object)) : object;

			_hash.insert(std::hash_map<Key, BaseStructure<Key, Object>::Data>::value_type(key, data));

			return object;
		}

		virtual Object* Find(const Key& key)
		{
			std::hash_map<Key, BaseStructure<Key, Object>::Data>::iterator iter = _hash.find(key);
			return iter == _hash.end() ? nullptr : GET_CONTENT_FROM_ITERATOR(iter);
		}

		virtual void Delete(const Key& key, bool contentRemove = false)
		{
			std::hash_map<Key, BaseStructure<Key, Object>::Data>::iterator iter = _hash.find(key);

			if( iter == _hash.end() )
				return;

			if(contentRemove)
				SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );

			_hash.erase(iter);
		}

		virtual void DeleteAll(bool contentRemove = false)
		{
			if(contentRemove)
			{
				for(std::hash_map<Key, BaseStructure<Key,Object>::Data>::iterator iter = _hash.begin();iter != _hash.end(); ++iter)
				{
					if( GET_IS_COPY_FROM_ITERATOR(iter) )
						SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );
				}
			}

			_hash.clear();
		}

		inline const std::hash_map<Key, BaseStructure<Key, Object>::Data>& GetHashMap() const { return _hash;}
		GET_ACCESSOR(Size, unsigned int, _hash.size());
	};

}