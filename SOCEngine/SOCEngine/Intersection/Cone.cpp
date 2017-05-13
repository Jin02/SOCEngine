#include "Cone.h"
#include "Sphere.h"
#include <math.h>

using namespace Math;
using namespace Intersection;

bool Cone::Intersects(const Sphere &sphere)
{
	return Intersects(sphere, (*this));
}

bool Cone::Intersects(const Sphere& sphere, const Cone& cone)
{
	float sinAngle = sinf(cone.degree);
	float cosAngle = cosf(cone.degree);

	float invSin = 1.0f / sinAngle;
	float cosSqr = cosAngle * cosAngle;

	Vector3 CmV = sphere.center - cone.axis;

	if (CmV.Length() < cone.range)
		return false;

	Vector3 D = CmV + (cone.axis * (sphere.radius * invSin));

	float DSqrLen = D.Length();
	float e = D.Dot(cone.axis);

	if (e > 0.0f && e * e >= DSqrLen * cosSqr)
	{
		float sinSqr = sinAngle * sinAngle;
		DSqrLen = CmV.x * CmV.x + CmV.y * CmV.y + CmV.z * CmV.z;
		e = -CmV.Dot(cone.axis);

		if (e > 0.0f && e * e >= DSqrLen * sinSqr)
		{
			float rSqr = sphere.radius * sphere.radius;
			return DSqrLen <= rSqr;
		}

		return true;
	}

	return false;
}
