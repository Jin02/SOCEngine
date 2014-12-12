#include "Sphere.h"

namespace Intersection
{
	Sphere::Sphere(void) : radius(0.0f), center(0, 0, 0)
	{
	}

	Sphere::Sphere(Math::Vector3 center, float radius) :
		center(center), radius(radius)
	{
	}

	bool Sphere::Intersects(const Sphere& sphere1, const Sphere& sphere2)
	{
		Math::Vector3 between = sphere1.center - sphere2.center;
		float distance = between.Length();

		if(distance < (sphere1.radius + sphere2.radius) )
			return false;

		return true;
	}

	bool Sphere::Intersects(const Sphere &sphere)
	{
		return Sphere::Intersects( (*this), sphere );
	}

	bool Sphere::Intersects(const Math::Vector3& center, float radius)
	{
		return Intersects(Sphere(center, radius));
	}
}