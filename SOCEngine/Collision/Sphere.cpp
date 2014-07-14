#include "Sphere.h"

namespace Collision
{
	Sphere::Sphere(void) : radius(0.0f), center(0, 0, 0)
	{
	}

	Sphere::Sphere(Math::Vector3 center, float radius) :
		center(center), radius(radius)
	{
	}

	bool Sphere::Intersection(const Sphere& sphere1, const Sphere& sphere2)
	{
		Math::Vector3 between = sphere1.center - sphere2.center;
		float distance = between.Length();

		if(distance < (sphere1.radius + sphere2.radius) )
			return false;

		return true;
	}

	bool Sphere::Intersection(Sphere &sphere)
	{
		return Sphere::Intersection( (*this), sphere );
	}
}