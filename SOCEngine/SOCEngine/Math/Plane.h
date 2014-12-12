#pragma once

#include "MathCommon.h"
#include "Vector3.h"

namespace Math
{
	class Plane
	{
	public:
		enum Direction{ FRONT, BACK, SAME };

	public:
		float a, b, c, d;

	public:
		Plane();
		Plane(float a, float b, float c, float d);
		~Plane();

	public:
		Plane& Normalize();
		float DistancePoint(const Vector3& p);
		float GetDistancePoint(const Vector3& v);
		bool SameSide(const Vector3& v);
		Direction GetSide(const Vector3& v);

	public:
		static void FromPoints(Plane& out, const Vector3& v1, const Vector3& v2, const Vector3& v3);
		static void FromPointNormal(Plane& out, const Vector3& point, const Vector3& normal);
		static float DotCoord(const Plane& p, const Vector3& v);
		static float DotNoraml(const Plane& p, const Vector3& v);
		static void Normalize(Plane& out, const Plane& p);
		static float GetDistancePoint(const Plane& p, const Vector3& v);
		static bool SameSide(const Plane& p, const Vector3& v);
		static Direction GetSide(const Plane& p, const Vector3& v);
	};

}