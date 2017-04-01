#include "Sphere.h"

using namespace Intersection;
using namespace Math;

Sphere::Sphere(void)
	: center(0.0f, 0.0f, 0.0f), radius(0.0f)
{
}

Sphere::Sphere(Vector3 center, float radius) :
	center(center), radius(radius)
{
}

bool Sphere::Intersects(const Sphere& sphere1, const Sphere& sphere2)
{
	Vector3 between = sphere1.center - sphere2.center;
	float distance = between.Length();

	if (distance <= (sphere1.radius + sphere2.radius))
		return true;

	return false;
}

bool Sphere::Intersects(const Sphere &sphere)
{
	return Sphere::Intersects((*this), sphere);
}

bool Sphere::Intersects(const Vector3& center, float radius)
{
	return Intersects(Sphere(center, radius));
}