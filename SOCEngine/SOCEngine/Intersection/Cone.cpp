#include "Cone.h"
#include "Sphere.h"
#include <math.h>

using namespace Math;
using namespace Intersection;

Cone::Cone(void) :
	degree(0), range(0), axis(0, 0, 0), vertex(0, 0, 0)
{

}

Cone::Cone(float _degree, float _range, const Vector3& _axis, const Vector3& _vertex)
	: degree(_degree), range(_range), axis(_axis), vertex(_vertex)
{
}

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

	Vector3 CmV = sphere.center - cone.vertex;

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