#include "Sphere.h"

using namespace Intersection;
using namespace Math;

bool Sphere::Intersects(const Sphere& sphere1, const Sphere& sphere2)
{
	Vector3 between	= sphere1.center - sphere2.center;
	float distance	= between.Length();

	return distance <= (sphere1.radius + sphere2.radius);
}

bool Sphere::Intersects(const Sphere &sphere)
{
	return Sphere::Intersects((*this), sphere);
}

bool Sphere::Intersects(const Vector3& center, float radius)
{
	return Intersects(Sphere(center, radius));
}
