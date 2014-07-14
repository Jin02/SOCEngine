#pragma once

#include <vector>
#include <string>

template <class Object>
class Container
{
protected:
	int order;
	bool clone;
	std::vector<Object*> objects;

public:
	std::string name;
	std::string tag;
	std::string layer;

public:
	Container(void)
	{
		order = 0;
		clone = false;
	}

	~Container(void)
	{
	}


private:
	enum FIND_ENUM{FIND_ENUM_NAME, FIND_ENUM_TAG};
	std::vector<Object*> _FindObject(std::string str, FIND_ENUM e, bool one)
	{
		std::vector<Object*> v;
		typename std::vector<Object*>::iterator iter;

		for(iter = objects.begin(); iter != objects.end(); ++iter)
		{
			std::string *findItem = e == FIND_ENUM_NAME ? &(*iter)->name : &(*iter)->tag;
			if( (*findItem) == str )
			{
				v.push_back((*iter));
				if(one) return v;
			}
		}
		return v;
	}

public:
	std::vector<Object*> FindObjects(std::string name)
	{
		return _FindObject(name, FIND_ENUM_NAME, false);
	}

	std::vector<Object*> FindObjectsWithTag(std::string tag)
	{
		return _FindObject(name, FIND_ENUM_TAG, false);
	}

	Object* Find(std::string name)
	{
		std::vector<Object*> v = _FindObject(name, FIND_ENUM_NAME, true);
		return v.size() == 0 ? NULL : v[0];
	}

	Object* FindWithTag(std::string tag)
	{
		std::vector<Object*> v = _FindObject(tag, FIND_ENUM_TAG, true);
		return v.size() == 0 ? NULL : v[0];
	}

	bool Has(Object *object)
	{
		std::vector<Object*> v;
		typename std::vector<Object*>::iterator iter;

		for(iter = objects.begin(); iter != objects.end(); ++iter)
		{
			if( (*iter) == object )
				return true;
		}

		return false;
	}

	Object* Add(Object *child, bool copy = false)
	{
		Object *c = copy == false ? child : new Object(*child);

		c->order = objects.size() != 0 ? (*(objects.end() - 1))->order + 1 : 0;
		c->clone = copy;
		objects.push_back(c);

		return c;
	}

	Object* Add(Object *child, int order, bool copy = false)
	{
		Object *c = copy == false ? child : new Object(*child);

		typename std::vector<Object*>::iterator iter;

		for(iter = objects.begin(); iter != objects.end(); ++iter)
		{
			if( (*iter)->order <= order )
			{
				c->order = (*iter)->order == order ? order + 1 : order;
				c->clone = copy;
				objects.insert(iter + 1, c);
				return c;
			}
		}

		return nullptr;
	}

	void Delete(Object *child, bool remove)
	{
		typename std::vector<Object*>::iterator iter;

		for(iter = objects.begin(); iter != objects.end(); ++iter)
		{
			if((*iter) == child)
			{
				if(remove)
					Utility::SAFE_DELETE(*iter);

				objects.erase(iter);
				return;
			}
		}

	}

	void DeleteAll(bool deleteClone)
	{
		for(typename std::vector<Object*>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
		{
			if( deleteClone && clone )
				Utility::SAFE_DELETE(*iter);
		}

		objects.clear();
	}

public:
	typename std::vector<Object*>::iterator GetBeginIter()
	{
		return objects.begin();
	}

	typename std::vector<Object*>::iterator GetEndIter()
	{
		return objects.end();
	}

	int GetCount()
	{
		return objects.size();
	}

	Object* Get(unsigned int index)
	{
		return *(objects.begin()+index); 
	}




};