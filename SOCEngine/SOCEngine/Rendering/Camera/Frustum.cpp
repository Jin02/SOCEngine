#include "Frustum.h"

namespace Rendering
{
	using namespace Camera;

	Frustum::Frustum(float gap)
		:_isMake(false), _gap(gap)
	{
	}

	Frustum::~Frustum(void)
	{
	}

	void Frustum::Make(const Math::Matrix &viewProjection)
	{
		Math::Matrix matInv;
		Math::Matrix::Inverse(matInv, viewProjection);

		_planeVertex[0].x = -1.0f;	_planeVertex[0].y = -1.0f;	_planeVertex[0].z = 0.0f;
		_planeVertex[1].x =  1.0f;	_planeVertex[1].y = -1.0f;	_planeVertex[1].z = 0.0f;
		_planeVertex[2].x =  1.0f;	_planeVertex[2].y = -1.0f;	_planeVertex[2].z = 1.0f;
		_planeVertex[3].x = -1.0f;	_planeVertex[3].y = -1.0f;	_planeVertex[3].z = 1.0f;
		_planeVertex[4].x = -1.0f;	_planeVertex[4].y =  1.0f;	_planeVertex[4].z = 0.0f;
		_planeVertex[5].x =  1.0f;	_planeVertex[5].y =  1.0f;	_planeVertex[5].z = 0.0f;
		_planeVertex[6].x =  1.0f;	_planeVertex[6].y =  1.0f;	_planeVertex[6].z = 1.0f;
		_planeVertex[7].x = -1.0f;	_planeVertex[7].y =  1.0f;	_planeVertex[7].z = 1.0f;

		for(int i=0; i<8; ++i)
			Math::Vector3::TransformCoord(_planeVertex[i], _planeVertex[i], matInv);

		_position = (_planeVertex[0] + _planeVertex[5]) / 2.0f;

		Math::Plane::FromPoints(_plane[0], _planeVertex[4], _planeVertex[7], _planeVertex[6]);	// 상 평면(top)
		Math::Plane::FromPoints(_plane[1], _planeVertex[0], _planeVertex[1], _planeVertex[2]);	// 하 평면(bottom)
		Math::Plane::FromPoints(_plane[2], _planeVertex[0], _planeVertex[4], _planeVertex[5]);	// 근 평면(near)
		Math::Plane::FromPoints(_plane[3], _planeVertex[2], _planeVertex[6], _planeVertex[7]);	// 원 평면(far)
		Math::Plane::FromPoints(_plane[4], _planeVertex[0], _planeVertex[3], _planeVertex[7]);	// 좌 평면(left)
		Math::Plane::FromPoints(_plane[5], _planeVertex[1], _planeVertex[5], _planeVertex[6]);	// 우 평면(right)

		_isMake = true;
	}

	bool Frustum::In(const Math::Vector3 &v, float radius) const
	{
		if( _isMake == false ) return false;

		float dist;

		for(int i=0; i<6; ++i)
		{
			dist = Math::Plane::DotCoord( _plane[i], v );
			if(dist < (radius + _gap) ) return false;
		}

		return true;
	}
}