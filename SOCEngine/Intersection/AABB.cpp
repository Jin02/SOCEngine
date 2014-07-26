#include "AABB.h"
#include "Common.h"
#include "Ray.h"

namespace Intersection
{
	AABB::AABB() :
		_center(0, 0, 0), _size(1, 1, 1)
	{
		_extents = _size / 2.0f;

		_min = -_extents;
		_max =  _extents;
	}

	AABB::AABB(const Math::Vector3& worldCenter, const Math::Vector3& size) :
		_center(worldCenter), _size(size)
	{
		_extents = size / 2.0f;

		_min = (worldCenter - _size) + _extents;
		_max = worldCenter + _extents;
	}

	AABB::~AABB(void)
	{
	}

	void AABB::SetMinMax(const Math::Vector3& min, const Math::Vector3& max)
	{
		_center -= (min + _extents);

		_min = min;
		_max = max;

		_size    = max - min;
		_extents = _size / 2.0f;
		_center += min + _extents;
	}

	void AABB::Expand(float amount)
	{
		_size.x += amount;
		_size.y += amount;
		_size.z += amount;
		
		_extents = _size / 2.0f;
		_min = _center - _extents;
		_max = _center + _extents;
	}

	void AABB::Expand(const Math::Vector3& amount)
	{
		_size += amount;
		_extents = _size / 2.0f;
		_min = _center - _extents;
		_max = _center + _extents;
	}

	bool AABB::Intersects(const AABB& bounds)
	{
		Math::Vector3 realMin = _center + _min;
		Math::Vector3 realMax = _center + _max;
		Math::Vector3 otherMin = bounds._center + _min;
		Math::Vector3 otherMax = bounds._center + _max;

		if(realMax.x < otherMin.x || realMin.x > otherMax.x) return false;
		if(realMax.y < otherMin.y || realMin.y > otherMax.y) return false;
		if(realMax.z < otherMin.z || realMin.z > otherMax.z) return false;

		return true;
	}

	float AABB::SqrDistance(const Math::Vector3& point)
	{
		Math::Vector3 v = point - _extents - _center;
		return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
	}

	bool AABB::Contains(const Math::Vector3& point)
	{
		Math::Vector3& realMin = _center + _min;
		Math::Vector3& realMax = _center + _max;

		if(point.x < realMin.x || point.x > realMax.x) return false;
		if(point.y < realMin.y || point.y > realMax.y) return false;
		if(point.z < realMin.z || point.z > realMax.z) return false;

		return true;
	}

	bool AABB::Intersects(const Ray& ray, Math::Vector3 *outPickPoint /*= NULL*/, float gap)
	{
		Math::Vector3 realMin = _center + _min;
		Math::Vector3 realMax = _center + _max;

		float rayMin = 0.0f;
		float rayMax = 100000.0f;

		for(int i=0; i<3; ++i)
		{
			if(abs(ray.direction[i]) < gap)
			{
				if(realMin[i] > ray.origin[i] || ray.origin[i] > realMax[i])
					return false;
			}
			else
			{
				float denom = 1.0f / ray.direction[i];
				float t1 = ( -ray.origin[i] - realMin[i]) * denom;
				float t2 = ( -ray.origin[i] - realMax[i]) * denom;

				if( t1 > t2 )
					std::swap(t1, t2);

				rayMin = MAX(rayMin, t1);
				rayMax = MIN(rayMax, t2);

				if(rayMin > rayMax)
					return false;
			}
		}

		if(outPickPoint)
			*outPickPoint = ray.origin + (ray.direction * rayMin);

		return true;
	}

	bool AABB::operator !=(const AABB &box)
	{
		if( _center != _center ) return true;
		if( _extents != _extents ) return true;

		return false;
	}

	bool AABB::operator ==(const AABB &box)
	{
		if( _center != _center ) return false;
		if( _extents != _extents ) return false;

		return true;
	}
}