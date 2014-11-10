#pragma once

#include "Common.h"
#include <memory>

class LPVoidType
{
private:
	void* _sysMem;

public:
	LPVoidType(void* sysMem = nullptr) : _sysMem(sysMem)
	{
	}

	LPVoidType(unsigned int size)
	{
		_sysMem = malloc(size);
	}

	~LPVoidType()
	{
		if(_sysMem)
			free(_sysMem);
	}
	
public:
	GET_ACCESSOR(Buffer, const void*, _sysMem);
	SET_ACCESSOR(Buffer, void*, _sysMem);
};