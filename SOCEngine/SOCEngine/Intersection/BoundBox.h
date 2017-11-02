#pragma once

#include "EngineMath.h"
#include "Common.h"

namespace Intersection
{
	class Ray;

	class BoundBox
	{
	public:
		BoundBox() = default;
		BoundBox(const Math::Vector3& worldCenter, const Math::Vector3& size);

		GET_CONST_ACCESSOR(Center,	const Math::Vector3&,	_center);
		GET_CONST_ACCESSOR(Size,	const Math::Vector3&,	_size);
		GET_CONST_ACCESSOR(Extents,	const Math::Vector3&,	_extents);
		GET_CONST_ACCESSOR(Min,		const Math::Vector3&,	_min);
		GET_CONST_ACCESSOR(Max,		const Math::Vector3&,	_max);

		void SetMinMax(const Math::Vector3& min, const Math::Vector3& max);
		void Expand(float amount);
		void Expand(const Math::Vector3& amount);

		bool Intersects(const BoundBox& bounds);
		bool Intersects(const Ray& ray, Math::Vector3* outPickPoint = nullptr, float gap = 0.0);
		bool Contains(const Math::Vector3& point);

		float SqrDistance(const Math::Vector3& point);

		bool operator!=(const BoundBox &box);
		bool operator==(const BoundBox &box);

	private:
		Math::Vector3 _center	= Math::Vector3(0.0f, 0.0f, 0.0f);
		Math::Vector3 _size		= Math::Vector3(1.0f, 1.0f, 1.0f);
		Math::Vector3 _extents	= _size / 2.0f;
		Math::Vector3 _min		= -_extents;
		Math::Vector3 _max		= _extents;
	};

}
