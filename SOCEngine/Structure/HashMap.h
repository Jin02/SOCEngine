#pragma once

#include "BaseStructure.h"
#include <hash_map>

namespace Structure
{
	template <class Object>
	class HashMap : public BaseStructure<Object>
	{
	private:
		std::hash_map<std::string, BaseStructure<Object>::Data> _hash;

	public:
		HashMap(void){}
		virtual ~HashMap(void)
		{
			DeleteAll(true);
		}

	public:
		virtual Object* Add(const std::string& key, Object* object, bool copy = false)
		{
			std::hash_map<std::string, BaseStructure<Object>::Data>::iterator iter = _hash.find(key);

			if(iter != _hash.end())
				return GET_CONTENT_FROM_ITERATOR(iter);

			BaseStructure<Object>::Data data;
			data.first = copy;
			data.second = copy ? new Object((*object)) : object;

			_hash.insert(std::hash_map<std::string, BaseStructure<Object>::Data>::value_type(key, data));

			return object;
		}

		virtual Object* Find(const std::string& key)
		{
			std::hash_map<std::string, BaseStructure<Object>::Data>::iterator iter = _hash.find(key);
			return iter == _hash.end() ? nullptr : GET_CONTENT_FROM_ITERATOR(iter);
		}

		virtual void Delete(const std::string& key, bool contentRemove = false)
		{
			std::hash_map<std::string, BaseStructure<Object>::Data>::iterator iter = _hash.find(key);

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
				for(std::hash_map<std::string, BaseStructure<Object>::Data>::iterator iter = _hash.begin();iter != _hash.end(); ++iter)
				{
					if( GET_IS_COPY_FROM_ITERATOR(iter) )
						SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );
				}
			}

			_hash.clear();
		}
	};

}