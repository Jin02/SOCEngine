#pragma once

#include "Common.h"
#include <string>

class Container
{
private:
	void*		_data;
	bool		_enableDealloc;

public:
	Container() : _data(nullptr), _enableDealloc(true)
	{
	}

	~Container()
	{
		if(_enableDealloc)
		{
			free(_data);
			_data = nullptr;
		}
	}

public:
	template<typename Type>
	void Init(const Type& value)
	{
		Type* node = (Type*)malloc( sizeof(Type) );
		(*node) = value;

		_data = node;
	}

	template<typename Type>
	const Type& GetData()
	{
		return *((Type*)_data);
	}

	template<typename Type>
	void SetData(const Type& value)
	{
		*((Type*)_data) = value;
	}

	GET_SET_ACCESSOR(EnableDealloc, bool, _enableDealloc);
};
