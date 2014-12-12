#pragma once

#include "EngineMath.h"

namespace Intersection
{
	class Ray
	{
	public:
		Math::Vector3 origin;
		Math::Vector3 direction;

	public:
		Ray(const Math::Vector3& origin, const Math::Vector3& direction);
		~Ray(void);

	public:
		const Math::Vector3 GetPoint(float distance);
	};
}