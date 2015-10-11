#pragma once

#include "EngineMath.h"
#include "Common.h"

namespace Intersection
{
	class Ray;

	class BoundBox
	{
	private:
		Math::Vector3 _center;
		Math::Vector3 _size;
		Math::Vector3 _extents;
		Math::Vector3 _min;
		Math::Vector3 _max;

	public:
		BoundBox();
		BoundBox(const Math::Vector3& worldCenter, const Math::Vector3& size);
		~BoundBox(void);

	public:
		void SetMinMax(const Math::Vector3& min, const Math::Vector3& max);
		void Expand(float amount);
		void Expand(const Math::Vector3& amount);

		bool Intersects(const BoundBox& bounds);
		bool Intersects(const Ray& ray, Math::Vector3* outPickPoint = nullptr, float gap = 0.0);
		bool Contains(const Math::Vector3& point);

		float SqrDistance(const Math::Vector3& point);

	public:
		bool operator!=(const BoundBox &box);
		bool operator==(const BoundBox &box);

	public:
		GET_ACCESSOR(Center,	const Math::Vector3&,	_center);
		GET_ACCESSOR(Size,		const Math::Vector3&,	_size);
		GET_ACCESSOR(Extents,	const Math::Vector3&,	_extents);
		GET_ACCESSOR(Min,		const Math::Vector3&,	_min);
		GET_ACCESSOR(Max,		const Math::Vector3&,	_max);
	};

}