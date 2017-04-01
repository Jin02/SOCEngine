#include "Ray.h"

using namespace Intersection;
using namespace Math;

Ray::Ray(const Vector3& _origin, const Vector3& _direction)
	: origin(_origin), direction(_direction)
{
}

const Vector3 Ray::GetPoint(float distance)
{
	return direction.Normalized() * distance;
}