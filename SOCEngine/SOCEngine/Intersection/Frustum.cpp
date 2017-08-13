#include "Frustum.h"

using namespace Math;
using namespace Intersection;

void Frustum::Make(const Matrix &viewProjection)
{
	Matrix matInv = Matrix::Inverse( viewProjection);

	_planeVertex[0].x = -1.0f;	_planeVertex[0].y = -1.0f;	_planeVertex[0].z = 0.0f;
	_planeVertex[1].x =  1.0f;	_planeVertex[1].y = -1.0f;	_planeVertex[1].z = 0.0f;
	_planeVertex[2].x =  1.0f;	_planeVertex[2].y = -1.0f;	_planeVertex[2].z = 1.0f;
	_planeVertex[3].x = -1.0f;	_planeVertex[3].y = -1.0f;	_planeVertex[3].z = 1.0f;
	_planeVertex[4].x = -1.0f;	_planeVertex[4].y =  1.0f;	_planeVertex[4].z = 0.0f;
	_planeVertex[5].x =  1.0f;	_planeVertex[5].y =  1.0f;	_planeVertex[5].z = 0.0f;
	_planeVertex[6].x =  1.0f;	_planeVertex[6].y =  1.0f;	_planeVertex[6].z = 1.0f;
	_planeVertex[7].x = -1.0f;	_planeVertex[7].y =  1.0f;	_planeVertex[7].z = 1.0f;

	for(int i=0; i<8; ++i)
		_planeVertex[i] = Vector3::TransformCoord(_planeVertex[i], matInv);

	_position = (_planeVertex[0] + _planeVertex[5]) / 2.0f;

	_plane[0] = Plane::FromPoints(_planeVertex[4], _planeVertex[7], _planeVertex[6]);	// 상 평면(top)
	_plane[1] = Plane::FromPoints(_planeVertex[0], _planeVertex[1], _planeVertex[2]);	// 하 평면(bottom)
	_plane[2] = Plane::FromPoints(_planeVertex[0], _planeVertex[4], _planeVertex[5]);	// 근 평면(near)
	_plane[3] = Plane::FromPoints(_planeVertex[2], _planeVertex[6], _planeVertex[7]);	// 원 평면(far)
	_plane[4] = Plane::FromPoints(_planeVertex[0], _planeVertex[3], _planeVertex[7]);	// 좌 평면(left)
	_plane[5] = Plane::FromPoints(_planeVertex[1], _planeVertex[5], _planeVertex[6]);	// 우 평면(right)
}

bool Frustum::In(const Vector3 &v, float radius) const
{
	for(uint i=0; i<6; ++i)
	{
		float dist = Plane::DotCoord(_plane[i], v);

		if(dist > radius)
			return false;
	}

	return true;
}
