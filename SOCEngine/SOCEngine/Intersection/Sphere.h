#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Sphere
	{
	public:
		Sphere(void);
		Sphere(Math::Vector3 center, float radius);

	public:
		static bool Intersects(const Sphere& sphere1, const Sphere& sphere2);
		bool Intersects(const Sphere& sphere);
		bool Intersects(const Math::Vector3& center, float radius);

	public:
		Math::Vector3 center;
		float		  radius;
	};

}