#include "Plane.h"
#include <math.h>

using namespace Math;

Plane::Plane(float _a, float _b, float _c, float _d)
	: a(_a), b(_b), c(_c), d(_d)
{

}

float Plane::DistancePoint(const Vector3& p) const
{
	return ComputeDistanceWithPoint((*this), p);
}

Plane Plane::FromPoints(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 edge1 = v2 - v1;
	Vector3 edge2 = v3 - v1;
	Vector3 normal = Vector3::Normalize( Vector3::Cross(edge1, edge2) );

	return FromPointNormal(v1, normal);
}

Plane Plane::FromPointNormal(const Vector3& point, const Vector3& normal)
{
	Plane out;

	out.a = normal.x;
	out.b = normal.y;
	out.c = normal.z;
	out.d = -Vector3::Dot(point, normal);

	return out;
}

float Plane::DotCoord(const Plane& p, const Vector3& v)
{
	return DotNoraml(p, v) + p.d;
}

float Plane::DotNoraml(const Plane& p, const Vector3& v)
{
	return (p.a * v.x) + (p.b * v.y) + (p.c * v.z);				 
}

Plane Plane::Normalize(const Plane& p)
{
	float norm = sqrtf( (p.a * p.a) + (p.b * p.b) + (p.c * p.c) );
	return (norm != 0.0f) ? Plane( p.a / norm, p.b / norm, p.c / norm, p.d / norm ) : Plane(0, 0, 0, 0);
}

float Plane::ComputeDistanceWithPoint(const Plane& p, const Vector3& v)
{
	return fabsf( DotCoord(p, v) );
}

bool Plane::SameSide(const Plane& p, const Vector3& v)
{
	return ComputeDistanceWithPoint(p, v) == 0.0f;
}

Plane::Direction Plane::GetSide(const Plane& p, const Vector3& v)
{
	float dist = DotCoord(p, v);
	
	if( dist < 0.0f )	return Direction::BACK;
	else if( dist > 0.0f )	return Direction::FRONT;
	return Direction::SAME;
}

float Plane::GetDistancePoint(const Vector3& v) const
{
	return ComputeDistanceWithPoint(*this, v);
}

bool Plane::SameSide(const Vector3& v) const
{
	return SameSide(*this, v);
}

Plane::Direction Plane::GetSide(const Vector3& v) const
{
	return GetSide(*this, v);
}
