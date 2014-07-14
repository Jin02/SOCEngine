#pragma once

#include <math.h>
#include <string>

namespace Math
{
#define PI 3.141592654f
#define EPSILON 1.0e-6

	class Vector3;

	class Common
	{
	public:
		static float Rad2Deg(float a);
		static float Deg2Rad(float a);

	public:
		static void EulerNormalize(Vector3& out, const Vector3& eulerAngels);
	};
}