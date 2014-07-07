#pragma once

#include <math.h>
#include <string>

namespace Math
{
#define PI 3.141592654f
#define EPSILON 1.0e-6

	class Common
	{
	public:
		static float Rad2Deg();
		static float Deg2Rad();
	};
}