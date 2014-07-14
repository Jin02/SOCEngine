#pragma once

#include "EngineMath.h"

namespace Collision
{
	class Ray;

	class AABB
	{
	private:
		Math::Vector3 _center;
		Math::Vector3 _size;
		Math::Vector3 _extents;
		Math::Vector3 _min;
		Math::Vector3 _max;

	public:
		AABB();
		AABB(const Math::Vector3& worldCenter, const Math::Vector3& size);
		~AABB(void);

	public:
		void SetMinMax(const Math::Vector3& min, const Math::Vector3& max);
		void Expand(float amount);
		void Expand(const Math::Vector3& amount);

		bool Intersects(const AABB& bounds);
		bool Contains(const Math::Vector3& point);
		
		float SqrDistance(const Math::Vector3& point);

		bool Intersects(const Ray& ray, Math::Vector3* outPickPoint = nullptr, float gap = 0.0);

	public:
		bool operator!=(const AABB &box);
		bool operator==(const AABB &box);
	};

}