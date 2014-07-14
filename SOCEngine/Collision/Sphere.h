#pragma once

#include "EngineMath.h"

namespace Collision
{
	class Sphere
	{
	public:
		Math::Vector3 center;
		float		  radius;

	public:
		Sphere(void);
		Sphere(Math::Vector3 center, float radius);

	public:
		static bool Intersection(const Sphere& sphere1, const Sphere& sphere2);
		bool Intersection(Sphere &sphere);
	};

}