#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Sphere;

	class Cone
	{
	public:
		Cone(void) = default;
		Cone(float degree, float range, const Math::Vector3& axis, const Math::Vector3& vertex);

		static bool Intersects(const Sphere &sphere, const Cone &cone);
		bool Intersects(const Sphere &sphere);

	public:
		float degree			= 0.0f;
		float range				= 0.0f;
		Math::Vector3 axis		= Math::Vector3(0.0f, 0.0f, 0.0f);
		Math::Vector3 vertex	= Math::Vector3(0.0f, 0.0f, 0.0f);
	};

}
