#include "BoundBox.h"
#include "Common.h"
#include "Ray.hpp"
#include <math.h>
#include <algorithm>

using namespace Intersection;
using namespace Math;

BoundBox::BoundBox(const Vector3& worldCenter, const Vector3& size) :
	_center(worldCenter), _size(size),
	_extents(_size / 2.0f), _min(-_extents), _max(_extents)
{
}

BoundBox BoundBox::Make(const Vector3& bbMin, const Vector3& bbMax)
{
	BoundBox result;
	result.SetMinMax(bbMin, bbMax);

	return result;
}

void BoundBox::SetMinMax(const Vector3& min, const Vector3& max)
{
	_min		= min;
	_max		= max;

	_size		= max - min;
	_size.x		= fabsf(_size.x);
	_size.y		= fabsf(_size.y);
	_size.z		= fabsf(_size.z);

	_extents	= _size / 2.0f;
	_center		= (max + min) / 2.0f;
}

void BoundBox::Expand(float amount)
{
	_size.x		+= amount;
	_size.y		+= amount;
	_size.z		+= amount;

	_extents	= _size / 2.0f;
	_min		= _center - _extents;
	_max		= _center + _extents;
}

void BoundBox::Expand(const Vector3& amount)
{
	_size		+= amount;
	_extents	= _size / 2.0f;
	_min		= _center - _extents;
	_max		= _center + _extents;
}

bool BoundBox::Intersects(const BoundBox& bounds) const
{
	Vector3 realMin		= _center + _min;
	Vector3 realMax		= _center + _max;
	Vector3 otherMin	= bounds._center + _min;
	Vector3 otherMax	= bounds._center + _max;

	bool isOutX = (realMax.x < otherMin.x) | (realMin.x > otherMax.x);
	bool isOutY = (realMax.y < otherMin.y) | (realMin.y > otherMax.y);
	bool isOutZ = (realMax.z < otherMin.z) | (realMin.z > otherMax.z);

	return (isOutX & isOutY & isOutZ) == false;
}

float BoundBox::SqrDistance(const Vector3& point) const
{
	Vector3 v = point - _extents - _center;
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

bool BoundBox::Contains(const Vector3& point) const
{
	Vector3 realMin = _center + _min;
	Vector3 realMax = _center + _max;

	bool isOutX = (point.x < realMin.x) | (point.x > realMax.x);
	bool isOutY = (point.y < realMin.y) | (point.y > realMax.y);
	bool isOutZ = (point.z < realMin.z) | (point.z > realMax.z);

	return (isOutX & isOutY & isOutZ) == false;
}

bool BoundBox::Intersects(const Ray& ray, Vector3 *outPickPoint, float gap) const
{
	Vector3 realMin = _center + _min;
	Vector3 realMax = _center + _max;

	float rayMin = 0.0f;
	float rayMax = 100000.0f;

	for (int i = 0; i < 3; ++i)
	{
		if (fabsf(ray.direction[i]) < gap)
		{
			if (	(realMin[i] > ray.origin[i]) | 
					(realMax[i] < ray.origin[i])	)
				return false;
		}
		else
		{
			float denom = 1.0f / ray.direction[i];
			float t1 = (-ray.origin[i] - realMin[i]) * denom;
			float t2 = (-ray.origin[i] - realMax[i]) * denom;

			if (t1 > t2)
				std::swap(t1, t2);

			rayMin = (rayMin > t1) ? rayMin : t1;
			rayMax = (rayMax < t2) ? rayMax : t2;

			if (rayMin > rayMax)
				return false;
		}
	}

	if (outPickPoint)
		*outPickPoint = ray.origin + (ray.direction * rayMin);

	return true;
}

bool BoundBox::operator !=(const BoundBox &box)
{
	return (_center != _center) & (_extents != _extents);
}

bool BoundBox::operator ==(const BoundBox &box)
{
	return operator!=(box);
}
