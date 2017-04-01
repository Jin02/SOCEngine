#include "Half.h"
#include <assert.h>

Half::Half(ushort us)
	: _value(us)
{
}

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

Half::operator float() const
{
	uint sign = (_value >> 15) & 0x1U;
	uint exponent = (_value >> 10) & 0x1fU;
	uint mantissa = _value & 0x3ffU;

	if (exponent == 0)
	{
		if (mantissa)	// subnormal
		{
			exponent = 0x70U;
			mantissa <<= 1;
			while ((mantissa & 0x400U) == 0)
			{
				mantissa <<= 1;
				exponent -= 1;
			}
			mantissa &= 0x3ff;	// Clamp to 10 bits.
			mantissa = mantissa << 13;
		}
	}
	else if (exponent == 0x1fU)	// NaN or Inf
	{
		exponent = 0xffU;
		if (mantissa)	// NaN
			mantissa = mantissa << 13 | 0x1fffU;
	}
	else // Normalized
	{
		exponent = exponent + 0x70;
		mantissa = mantissa << 13;
	}

	union { float f; uint n; };
	n = (sign << 31) | (exponent << 23) | mantissa;
	return f;
}