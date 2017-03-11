#include "Half.h"
#include <assert.h>

Half::Half(float f)
{
	// s = sign, e = exponent, m = mantissa
	// 32bit float
	// 1    8               23
	// s eeeeeeee mmmmmmmmmmmmmmmmmmmmmmm
	// 16bit float (half)
	// 1   5       10
	// s eeeee mmmmmmmmmm

	ushort floatBit = *(reinterpret_cast<uint*>(&f));

	int exponent = static_cast<int>((floatBit & 0x7F800000) >> 23) - 127 + 15;
	assert(exponent < 31);

	if (exponent <= 0)
	{
		_value = static_cast<ushort>((floatBit & 0x80000000) >> 16);
		return;
	}

	uint outSign		= (floatBit & 0x80000000) >> 16;
	uint outExp			= static_cast<uint>(exponent) << 10;
	uint outMantissa	= (floatBit & 0x007FFFFF) >> 13;

	_value = (outSign | outExp | outMantissa);
}