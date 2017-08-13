#pragma once

#include "Vector3.h"

namespace Math
{
	class Plane
	{
	public:
		enum class Direction { FRONT, BACK, SAME };
		Plane(float a = 0.0f, float b = 0.0f, float c = 0.0f, float d = 0.0f);

		const Plane		Normalized() const { return Plane::Normalize((*this)); }
		float			DistancePoint(const Vector3& p) const;
		float			GetDistancePoint(const Vector3& v) const;
		bool			SameSide(const Vector3& v) const;
		Direction		GetSide(const Vector3& v) const;

		static Plane		FromPoints(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		static Plane		FromPointNormal(const Vector3& point, const Vector3& normal);
		static float		DotCoord(const Plane& p, const Vector3& v);
		static float		DotNoraml(const Plane& p, const Vector3& v);
		static Plane		Normalize(const Plane& p);
		static float		ComputeDistanceWithPoint(const Plane& p, const Vector3& v);
		static bool			SameSide(const Plane& p, const Vector3& v);
		static Direction	GetSide(const Plane& p, const Vector3& v);

	public:
		float a, b, c, d;
	};

}
