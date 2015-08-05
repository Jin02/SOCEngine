#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Sphere;

	class Cone
	{
	public:
		float degree;
		float range;
		Math::Vector3 axis;
		Math::Vector3 vertex; //vertex

	public:
		Cone(void);
		Cone(float degree, float range, const Math::Vector3 &axis, const Math::Vector3 &vertex);

	public:
		static bool Intersects(const Sphere &sphere, const Cone &cone);
		bool Intersects(const Sphere &sphere);
	};

}