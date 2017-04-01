#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Ray
	{
	public:
		Ray() = default;
		Ray(const Math::Vector3& _origin, const Math::Vector3& _direction) : origin(_origin), direction(_direction) { }
		const Math::Vector3 GetPoint(float distance)
		{
			return direction.Normalized() * distance;
		}

	public:
		Math::Vector3 origin		= Math::Vector3(0.0f, 0.0f, 0.0f);
		Math::Vector3 direction		= Math::Vector3(0.0f, 0.0f, 0.0f);
	};
}
