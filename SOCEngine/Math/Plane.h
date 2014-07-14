#pragma once

#include "MathCommon.h"
#include "Vector3.h"

namespace Math
{
	class Plane
	{
	public:
		float a, b, c, d;

	public:
		Plane() : a(0), b(0), c(0), d(0)
		{
		
		}

		Plane(float a, float b, float c, float d)
		{

		}

		~Plane()
		{

		}

	public:

		Vector3 Normal()
		{
		}

		float Distance(const Vector3& p)
		{

		}


	public:
		static void FromPoints(Plane& out, const Vector3& v1, const Vector3& v2, const Vector3& v3)
		{
			Vector3 edge1 = v2 - v1;
			Vector3 edge2 = v3 - v1;
			Vector3 normal = Vector3::Normalize( Vector3::Cross(edge1, edge2) );
			FromPointNormal(out, v1, normal);
		}

		static void FromPointNormal(Plane& out, const Vector3& point, const Vector3& normal)
		{
			out.a = normal.x;
			out.b = normal.y;
			out.c = normal.z;
			out.d = -Vector3::Dot(point, normal);
		}

		static float DotCoord(const Plane& p, const Vector3& v)
		{
			return DotNoraml(p, v) + p.d;
		}

		static float DotNoraml(const Plane& p, const Vector3& v)
		{
			return (p.a * v.x) + (p.b * v.y) + (p.c * v.z);				 
		}

		static void Normalize(Plane& out, const Plane& p)
		{
			float norm = sqrtf( (p.a * p.a) + (p.b * p.b) + (p.c * p.c) );
			if(norm != 0.0f)
				out = Plane( p.a / norm, p.b / norm, p.c / norm, p.d / norm );
			else
				out = Plane(0, 0, 0, 0);
		}

		static float GetDistancePoint(const Plane& p, const Vector3& v)
		{
			return abs(p.a * v.x + p.b * v.y + p.c * v.z + p.d) / sqrtf(p.a*p.a+p.b*p.b+p.c*p.c);
		}

		static bool SameSide(const Plane& p, const Vector3& v)
		{
			if( DotCoord(p, v) == 0.0f )
				return true;

			return false;
		}

		static bool GetSide(const Plane& p, const Vector3& v)
		{
			float dist = DotCoord(p, v);

			if( dist <= 0 )
				return false;
			
			return true;
		}
	};

}