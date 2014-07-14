#pragma once

#include "EngineMath.h"
#include "Common.h"

namespace Rendering
{
	class Frustum
	{
	private:
		Math::Vector3		_planeVertex[8];
		Math::Vector3		_position;
		Math::Plane			_plane[6];
		float				_gap;
		bool				_isMake;

	public:
		Frustum(float gap = 0.0);
		~Frustum(void);

	public:
		void Make(const Math::Matrix &viewProjection);
		bool In(const Math::Vector3 &v, float radius = 0.0f);

	public:
		GET_ACCESSOR(Position, const Math::Vector3, _position);
	};

}