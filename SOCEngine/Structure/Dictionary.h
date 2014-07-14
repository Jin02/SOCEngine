#pragma once

#include "HashMap.h"
#include "Utility.h"
#include <string>

template <class Object>
class Dictionary
{
private:
	HashMap<std::string, Object*> hash;

public:
	Dictionary(void)
	{
	}

	~Dictionary(void)
	{
		DeleteAll();
	}

	Object* Add(std::string key, Object* object)
	{
		SOCHashMap<std::string, Object*>::iterator iter = hash.find(key);

		if(iter != hash.end())
			return iter->second;

		hash.insert(SOCHashMap<std::string, Object*>::value_type(key, object));

		return object;
	}

	Object* Find(std::string key)
	{
		SOCHashMap<std::string, Object*>::iterator iter = hash.find(key);
		return iter == hash.end() ? nullptr : iter->second;
	}

	void Delete(std::string key)
	{
		SOCHashMap<std::string, Object*>::iterator iter = hash.find(key);

		if( iter == hash.end() )
			return;

		Utility::SAFE_DELETE(iter->second);

		hash.erase(iter);
	}

	void Delete(Object* object, bool dealloc)
	{
		SOCHashMap<std::string, Object*>::iterator iter;
		for(iter = hash.begin(); iter != hash.end(); ++iter)
		{
			if( (*iter).second == object )
			{
				if(dealloc)
					delete object;

				hash.erase(iter);
				return;
			}
		}
	}

	void DeleteAll()
	{
		for(SOCHashMap<std::string, Object*>::iterator iter = hash.begin();iter != hash.end(); ++iter)
			Utility::SAFE_DELETE(iter->second);

		hash.clear();
	}
};