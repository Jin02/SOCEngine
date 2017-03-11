#pragma once

#include "Common.h"

class Half final
{
public:
	Half(float f);
	GET_ACCESSOR(Value, ushort, _value);

private:
	ushort _value;
};