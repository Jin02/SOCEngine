#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(Math::Vector3 _center, float _radius) : center(_center), radius(_radius) {} ;

		static bool Intersects(const Sphere& sphere1, const Sphere& sphere2);
		bool Intersects(const Sphere& sphere);
		bool Intersects(const Math::Vector3& center, float radius);

		Math::Vector3	center = Math::Vector3(0.0f, 0.0f, 0.0f);
		float		radius = 0.0f;
	};

}
