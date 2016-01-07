#pragma once

#include <math.h>
#include <string>

namespace Math
{
#define MATH_PI 3.141592654f
	
	class Vector3;

	class Common
	{
	public:
		static float Rad2Deg(float a);
		static float Deg2Rad(float a);
		static unsigned short FloatToHalf(float f);

	public:
		static void EulerNormalize(Vector3& out, const Vector3& eulerAngles);
		static float AdjustFraction(float f);
	};
}