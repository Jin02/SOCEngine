#include "Cone.h"
#include "Sphere.h"

namespace Collision
{
	Cone::Cone(void) :
		angle(0), range(0), axis(0, 0, 0), vertex(0, 0, 0)
	{

	}

	Cone::Cone(float _angle, float _range, const Math::Vector3& _axis, const Math::Vector3& _vertex)
		: angle(_angle), range(_range), axis(_axis), vertex(_vertex)
	{
	}

	bool Cone::Intersection(const Sphere &sphere)
	{
		return Intersection(sphere, (*this));
	}

	bool Cone::Intersection(const Sphere& sphere, const Cone& cone)
	{
		float sinAngle = sin(cone.angle);
		float cosAngle = cos(cone.angle);

		float invSin = 1.0f / sinAngle;
		float cosSqr = cosAngle * cosAngle;

		Math::Vector3 CmV = sphere.center - cone.vertex;

		if(CmV.Length() < cone.range)
			return false;

		Math::Vector3 D = CmV + (cone.axis * (sphere.radius * invSin));

		float DSqrLen = D.Length();
		float e = D.Dot(cone.axis);

		if( e > 0.0f && e * e >= DSqrLen * cosSqr )
		{
			float sinSqr = sinAngle * sinAngle;
			DSqrLen = CmV.x * CmV.x + CmV.y * CmV.y + CmV.z * CmV.z;
			e = -CmV.Dot(cone.axis);

			if( e > 0.0f && e * e >= DSqrLen * sinSqr )
			{
				float rSqr = sphere.radius * sphere.radius;
				return DSqrLen <= rSqr;
			}

			return true;
		}

		return false;
	}
}