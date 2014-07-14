#include "Ray.h"


namespace Collision
{
	Ray::Ray(const Math::Vector3& _origin, const Math::Vector3& _direction)
		: origin(_origin), direction(_direction)
	{
	}

	Ray::~Ray(void)
	{
	}

	const Math::Vector3 Ray::GetPoint(float distance)
	{
		Math::Vector3 dir = Math::Vector3::Normalize(direction);
		dir *= distance;

		return dir;
	}

}