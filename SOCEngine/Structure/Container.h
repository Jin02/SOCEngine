#pragma once

#include <vector>
#include <string>
#include "Common.h"

template <class Object>
class Container
{
protected:
	int _order;
	bool _clone;
	std::vector<Object*> _objects;

	std::string _name;
	std::string _tag;
	std::string _layer;

public:
	Container(void)
	{
		_order = 0;
		_clone = false;
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

		for(iter = _objects.begin(); iter != _objects.end(); ++iter)
		{
			std::string *findItem = e == FIND_ENUM_NAME ? &(*iter)->_name : &(*iter)->_tag;
			if( (*findItem) == str )
			{
				v.push_back((*iter));
				if(one) return v;
			}
		}
		return v;
	}

public:
	std::vector<Object*> FindObjects(std::string _name)
	{
		return _FindObject(_name, FIND_ENUM_NAME, false);
	}

	std::vector<Object*> FindObjectsWithTag(std::string _tag)
	{
		return _FindObject(_name, FIND_ENUM_TAG, false);
	}

	Object* Find(std::string _name)
	{
		std::vector<Object*> v = _FindObject(_name, FIND_ENUM_NAME, true);
		return v.size() == 0 ? NULL : v[0];
	}

	Object* FindWithTag(std::string _tag)
	{
		std::vector<Object*> v = _FindObject(_tag, FIND_ENUM_TAG, true);
		return v.size() == 0 ? NULL : v[0];
	}

	bool Has(Object *object)
	{
		std::vector<Object*> v;
		typename std::vector<Object*>::iterator iter;

		for(iter = _objects.begin(); iter != _objects.end(); ++iter)
		{
			if( (*iter) == object )
				return true;
		}

		return false;
	}

	Object* Add(Object *child, bool copy = false)
	{
		Object *c = copy == false ? child : new Object(*child);

		c->_order = _objects.size() != 0 ? (*(_objects.end() - 1))->_order + 1 : 0;
		c->_clone = copy;
		_objects.push_back(c);

		return c;
	}

	Object* Add(Object *child, int _order, bool copy = false)
	{
		Object *c = copy == false ? child : new Object(*child);

		typename std::vector<Object*>::iterator iter;

		for(iter = _objects.begin(); iter != _objects.end(); ++iter)
		{
			if( (*iter)->_order <= _order )
			{
				c->_order = (*iter)->_order == _order ? _order + 1 : _order;
				c->_clone = copy;
				_objects.insert(iter + 1, c);
				return c;
			}
		}

		return nullptr;
	}

	void Delete(Object *child, bool remove)
	{
		typename std::vector<Object*>::iterator iter;

		for(iter = _objects.begin(); iter != _objects.end(); ++iter)
		{
			if((*iter) == child)
			{
				if(remove)
					Utility::SAFE_DELETE(*iter);

				_objects.erase(iter);
				return;
			}
		}

	}

	void DeleteAll(bool deleteClone)
	{
		for(typename std::vector<Object*>::iterator iter = _objects.begin(); iter != _objects.end(); ++iter)
		{
			if( deleteClone && _clone )
				SAFE_DELETE(*iter);
		}

		_objects.clear();
	}

public:
	typename std::vector<Object*>::iterator GetBeginIter()
	{
		return _objects.begin();
	}

	typename std::vector<Object*>::iterator GetEndIter()
	{
		return _objects.end();
	}

	int GetCount()
	{
		return _objects.size();
	}

	Object* Get(unsigned int index)
	{
		return *(_objects.begin()+index); 
	}

	GET_ACCESSOR(Name, const std::string&, _name);
	GET_ACCESSOR(Tag,  const std::string&, _tag);
	GET_ACCESSOR(Layer, const std::string&, _layer);
};