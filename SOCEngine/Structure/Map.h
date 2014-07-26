#pragma once

#include <map>
#include "BaseStructure.h"

namespace Structure
{
	template <class Object>
	class Map : public BaseStructure<Object>
	{
	private:
		std::map<std::string, BaseStructure<Object>::Data> _map;

	public:
		Map(void){}
		virtual ~Map(void)
		{
			DeleteAll(true);
		}

	public:
		virtual Object* Add(const std::string& key, Object* object, bool copy = false)
		{
			std::map<std::string, BaseStructure<Object>::Data>::iterator iter = _map.find(key);

			if(iter != _map.end())
				return GET_CONTENT_FROM_ITERATOR(iter);

			BaseStructure<Object>::Data data;
			data.first = copy;
			data.second = copy ? new Object((*object)) : object;

			_map.insert(std::map<std::string, BaseStructure<Object>::Data>::value_type(key, data));

			return object;
		}

		virtual Object* Find(const std::string& key)
		{
			std::map<std::string, BaseStructure<Object>::Data>::iterator iter = _map.find(key);
			return iter == _map.end() ? nullptr : GET_CONTENT_FROM_ITERATOR(iter);
		}

		virtual void Delete(const std::string& key, bool contentRemove = false)
		{
			std::map<std::string, BaseStructure<Object>::Data>::iterator iter = _map.find(key);

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
				typename std::map<std::string, BaseStructure<Object>::Data>::iterator iter;
				for(iter = _map.begin();iter != _map.end(); ++iter)
				{
					if( GET_IS_COPY_FROM_ITERATOR(iter) )
						SAFE_DELETE( GET_CONTENT_FROM_ITERATOR(iter) );
				}
			}

			_map.clear();
		}
	};
}