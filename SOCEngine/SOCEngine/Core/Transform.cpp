#include "Transform.h"
#include <assert.h>

using namespace Core;
using namespace Math;

static void MakeRotationMatrix(	Matrix& outRotMat,
								const Vector3& right, const Vector3& up, const Vector3& forward	)
{
	outRotMat._m[0][0] = right.x;
	outRotMat._m[0][1] = up.x;
	outRotMat._m[0][2] = forward.x;
	outRotMat._m[0][3] = 0.0f;

	outRotMat._m[1][0] = right.y;
	outRotMat._m[1][1] = up.y;
	outRotMat._m[1][2] = forward.y;
	outRotMat._m[1][3] = 0.0f;

	outRotMat._m[2][0] = right.z;
	outRotMat._m[2][1] = up.z;
	outRotMat._m[2][2] = forward.z;
	outRotMat._m[2][3] = 0.0f;

	outRotMat._m[3][0] = 0;
	outRotMat._m[3][1] = 0;
	outRotMat._m[3][2] = 0;
	outRotMat._m[3][3] = 1.0f;
}

const Matrix & Transform::ComputeLocalMatrix()
{
	_localMat._11 = _scale.x * _right.x;
	_localMat._12 = _scale.y * _up.x;
	_localMat._13 = _scale.z * _forward.x;
	_localMat._14 = 0.0f;

	_localMat._21 = _scale.x * _right.y;
	_localMat._22 = _scale.y * _up.y;
	_localMat._23 = _scale.z * _forward.y;
	_localMat._24 = 0.0f;

	_localMat._31 = _scale.x * _right.z;
	_localMat._32 = _scale.y * _up.z;
	_localMat._33 = _scale.z * _forward.z;
	_localMat._34 = 0.0f;

	_localMat._41 = _position.x;
	_localMat._42 = _position.y;
	_localMat._43 = _position.z;
	_localMat._44 = 1.0f;

	return _localMat;
}

void Transform::UpdatePosition(const Vector3& pos)
{
	_position = pos;
	SetDirty();
}

void Transform::UpdateScale(const Vector3& scale)
{
	_scale = scale;
	SetDirty();
}

void Transform::UpdateRight(const Vector3& r)
{
	LookAtDir(Vector3::Cross(_up, r));
}

void Transform::UpdateUp(const Vector3& u)
{
	LookAtDir(Vector3::Cross(u, _right));
}

void Transform::UpdateForward(const Vector3& f)
{
	LookAtDir(f);
}

void Transform::UpdateEulerAngle(const Vector3 & e)
{
	_eulerAngle = e;
	Quaternion::FromEuler(_quaternion, -e);

	_right		= _quaternion.GetRight();
	_up			= _quaternion.GetUp();
	_forward	= _quaternion.GetForward();

	SetDirty();
}

void Transform::UpdateQuaternion(const Quaternion & q)
{
	_quaternion = q;

	Matrix rotationMatrix;
	Matrix::RotateUsingQuaternion(rotationMatrix, _quaternion);
	Vector3::FromRotationMatrix(_eulerAngle, rotationMatrix);

	_right		= _quaternion.GetRight();
	_up			= _quaternion.GetUp();
	_forward	= _quaternion.GetForward();

	SetDirty();
}

void Transform::LookAtPos(const Vector3 & targetPos, const Vector3* upVec)
{
	Vector3 position	= GetPosition();
	Vector3 forward		= (targetPos - position).Normalized();

	LookAtDir(forward, upVec);
}

void Transform::LookAtDir(const Vector3 & targetDir, const Vector3* upVec)
{
	Vector3 up = upVec ? *upVec : Vector3::Up();

	_forward	= targetDir.Normalized();
	_right		= Vector3::Cross(up, _forward);
	_up			= Vector3::Cross(_forward, _right);

	Matrix rotMat;
	MakeRotationMatrix(rotMat, _right, _up, _forward);

	Vector3::FromRotationMatrix(_eulerAngle, rotMat);
	Quaternion::FromRotationMatrix(_quaternion, rotMat);

	SetDirty();
}

const Vector3 Transform::GetWorldPosition() const
{
	return Vector3(_worldMat._41, _worldMat._42, _worldMat._43);
}

const Vector3 Transform::GetWorldScale() const
{
	return Vector3(	Vector3(_worldMat._11, _worldMat._12, _worldMat._13).Length(),
					Vector3(_worldMat._21, _worldMat._22, _worldMat._23).Length(),
					Vector3(_worldMat._31, _worldMat._32, _worldMat._33).Length() );
}

const Vector3 Transform::GetWorldRight(const Vector3& scale) const
{
	return Vector3(	_worldMat._11 / scale.x,
					_worldMat._21 / scale.y,
					_worldMat._31 / scale.z	).Normalized();
}

const Vector3 Transform::GetWorldUp(const Vector3& scale) const
{
	return Vector3(	_worldMat._12 / scale.x,
					_worldMat._22 / scale.y,
					_worldMat._32 / scale.z	).Normalized();
}

const Vector3 Transform::GetWorldForward(const Vector3& scale)	const
{
	return Vector3(	_worldMat._13 / scale.x,
					_worldMat._23 / scale.y,
					_worldMat._33 / scale.z	).Normalized();
}

const Vector3 Transform::FetchWorldEulerAngle() const
{
	Vector3 scale = GetWorldScale();
	Matrix rotMat;
	MakeRotationMatrix(rotMat, GetWorldRight(scale), GetWorldUp(scale), GetWorldForward(scale));

	Vector3 euler;
	Vector3::FromRotationMatrix(euler, rotMat);

	return euler;
}

const Quaternion Transform::FetchWorldQuaternion() const
{
	Vector3 scale = GetWorldScale();
	Matrix rotMat;
	MakeRotationMatrix(rotMat, GetWorldRight(scale), GetWorldUp(scale), GetWorldForward(scale));

	Quaternion q;
	Quaternion::FromRotationMatrix(q, rotMat);

	return q;
}


bool Transform::HasChild(ObjectId id) const
{
	for (const auto& iter : _childIds)
	{
		if(iter == id)
			return true;
	}

	return false;
}

void Transform::DeleteChild(ObjectId id)
{
	uint pos = 0;
	for (pos; pos < _childIds.size() && _childIds[pos] != id; ++pos);

	assert(pos == _childIds.size());
	_childIds.erase(_childIds.begin() + pos);
}


void Transform::UpdateDirty(TransformPool& pool)
{
	for (ObjectId id = _objectId; id != ObjectId::Undefined(); id = _parentId)
		pool.Find(id)->_dirty |= _dirty;
}

void Transform::_ComputeWorldMatrix(TransformPool& pool)
{
	for (auto childId : _childIds)
	{
		auto child = *pool.Find(childId);

		child._worldMat = child.ComputeLocalMatrix() * _worldMat;
		child._ComputeWorldMatrix(pool);
	}
}

void Transform::ComputeWorldMatrix(TransformPool& pool)
{
	// this func must be run in root
	assert(_parentId == ObjectId::Undefined());

	if(_dirty == false)	return;
	_worldMat = ComputeLocalMatrix();

	_ComputeWorldMatrix(pool);
}
