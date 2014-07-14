#pragma once

#include "EngineMath.h"

namespace Collision
{
	class Sphere;

	class Cone
	{
	public:
		float angle;
		float range;
		Math::Vector3 axis;
		Math::Vector3 vertex; //vertex

	public:
		Cone(void);
		Cone(float angle, float range, const Math::Vector3 &axis, const Math::Vector3 &vertex);

	public:
		static bool Intersection(const Sphere &sphere, const Cone &cone);
		bool Intersection(const Sphere &sphere);
	};

}