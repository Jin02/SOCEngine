#include "MathCommon.h"
#include "Vector3.h"
#include "Utility.h"

namespace Math
{
	float Common::Rad2Deg(float a)
	{
		return 180.0f / MATH_PI * a;
	}

	float Common::Deg2Rad(float a)
	{
		return MATH_PI / 180.0f * a;
	}

	void Common::EulerNormalize(Vector3& out, const Vector3& eulerAngles)
	{
		int x = (int)(eulerAngles.x / 360.0f);
		int y = (int)(eulerAngles.y / 360.0f);
		int z = (int)(eulerAngles.z / 360.0f);

		out.x = eulerAngles.x - x * 360.0f + (int)(eulerAngles.x < 0) * 360.0f;
		out.y = eulerAngles.y - y * 360.0f + (int)(eulerAngles.y < 0) * 360.0f;
		out.z = eulerAngles.z - z * 360.0f + (int)(eulerAngles.z < 0) * 360.0f;
	}

	unsigned short Common::FloatToHalf(float f)
	{
		// s = sign, e = exponent, m = mantissa
		// 32bit float
		// 1    8               23
		// s eeeeeeee mmmmmmmmmmmmmmmmmmmmmmm
		// 16bit float (half)
		// 1   5       10
		// s eeeee mmmmmmmmmm

		unsigned int floatBit = *((unsigned int*)&f);

		int exponent = (int)((floatBit &0x7F800000) >> 23) - 127 + 15;
		ASSERT_MSG_IF(exponent < 31, "Strange Exp");

		if(exponent <= 0)
			return (unsigned short)((floatBit & 0x80000000) >> 16);

		unsigned int outSign		= (floatBit & 0x80000000) >> 16;
		unsigned int outExp			= ((unsigned int)exponent) << 10;
		unsigned int outMantissa	= (floatBit & 0x007FFFFF) >> 13;

		return (unsigned short)(outSign | outExp | outMantissa);
	}

	float Common::AdjustFraction(float f)
	{
		int intValue = (int)(f + ((f < 0.0f) ? -FLT_EPSILON : FLT_EPSILON));

		float frac = (float)f - intValue;
		frac = (abs(frac) < FLT_EPSILON) ? 0.0f : frac;

		return (float)intValue + frac;
	}
}