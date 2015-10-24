#include "BoundBox.h"
#include "Common.h"
#include "Ray.h"

namespace Intersection
{
	BoundBox::BoundBox() :
		_center(0, 0, 0), _size(1, 1, 1)
	{
		_extents = _size / 2.0f;

		_min = -_extents;
		_max =  _extents;
	}

	BoundBox::BoundBox(const Math::Vector3& worldCenter, const Math::Vector3& size) :
		_center(worldCenter), _size(size)
	{
		_extents = size / 2.0f;

		_min = (worldCenter - _size) + _extents;
		_max = worldCenter + _extents;
	}

	BoundBox::~BoundBox(void)
	{
	}

	void BoundBox::SetMinMax(const Math::Vector3& min, const Math::Vector3& max)
	{
		_min = min;
		_max = max;

		_size	= max - min;
		_size.x	= abs(_size.x);
		_size.y	= abs(_size.y);
		_size.z	= abs(_size.z);

		_extents = _size / 2.0f;
		_center = (max - min) / 2.0f;
	}

	void BoundBox::Expand(float amount)
	{
		_size.x += amount;
		_size.y += amount;
		_size.z += amount;
		
		_extents = _size / 2.0f;
		_min = _center - _extents;
		_max = _center + _extents;
	}

	void BoundBox::Expand(const Math::Vector3& amount)
	{
		_size += amount;
		_extents = _size / 2.0f;
		_min = _center - _extents;
		_max = _center + _extents;
	}

	bool BoundBox::Intersects(const BoundBox& bounds)
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

	float BoundBox::SqrDistance(const Math::Vector3& point)
	{
		Math::Vector3 v = point - _extents - _center;
		return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
	}

	bool BoundBox::Contains(const Math::Vector3& point)
	{
		Math::Vector3& realMin = _center + _min;
		Math::Vector3& realMax = _center + _max;

		if(point.x < realMin.x || point.x > realMax.x) return false;
		if(point.y < realMin.y || point.y > realMax.y) return false;
		if(point.z < realMin.z || point.z > realMax.z) return false;

		return true;
	}

	bool BoundBox::Intersects(const Ray& ray, Math::Vector3 *outPickPoint /*= NULL*/, float gap)
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

				rayMin = std::max(rayMin, t1);
				rayMax = std::min(rayMax, t2);

				if(rayMin > rayMax)
					return false;
			}
		}

		if(outPickPoint)
			*outPickPoint = ray.origin + (ray.direction * rayMin);

		return true;
	}

	bool BoundBox::operator !=(const BoundBox &box)
	{
		if( _center != _center ) return true;
		if( _extents != _extents ) return true;

		return false;
	}

	bool BoundBox::operator ==(const BoundBox &box)
	{
		if( _center != _center ) return false;
		if( _extents != _extents ) return false;

		return true;
	}
}