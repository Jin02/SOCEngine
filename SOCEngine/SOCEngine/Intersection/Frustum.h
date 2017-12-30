#pragma once

#include "EngineMath.h"
#include "Common.h"
#include <array>

namespace Intersection
{
	class Frustum
	{
	public:
		Frustum() = default;
		GET_CONST_ACCESSOR(Position, const Math::Vector3&, _position);

		void Make(const Math::Matrix& viewProjection);
		bool In(const Math::Vector3& v, float radius = 0.0f) const;

	private:
		std::array<Math::Plane, 6>		_plane;
		std::array<Math::Vector3, 8>	_planeVertex;
		Math::Vector3					_position	= Math::Vector3(0.0f, 0.0f, 0.0f);
	};
}
