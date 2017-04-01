#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Ray
	{
	public:
		Ray(const Math::Vector3& origin, const Math::Vector3& direction);
		const Math::Vector3 GetPoint(float distance);

	public:
		Math::Vector3 origin;
		Math::Vector3 direction;
	};
}