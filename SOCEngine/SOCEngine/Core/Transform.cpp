#include "Transform.h"
#include <assert.h>

using namespace Core;
using namespace Math;

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
	_quaternion = Quaternion::FromEuler(-e);

	_right		= _quaternion.GetRight();
	_up			= _quaternion.GetUp();
	_forward	= _quaternion.GetForward();

	SetDirty();
}

void Transform::UpdateQuaternion(const Quaternion & q)
{
	_quaternion = q;

	Matrix rotationMatrix = Matrix::RotateUsingQuaternion(_quaternion);
	_eulerAngle = Vector3::FromRotationMatrix(rotationMatrix);

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
	Matrix rotMat = Matrix::LookAtDir(targetDir, upVec);

	_eulerAngle = Vector3::FromRotationMatrix(rotMat);
	_quaternion = Quaternion::FromRotationMatrix(rotMat);

	SetDirty();
}

void Transform::AddChild(Transform& child)
{
	bool hasParent = child._parentID.Literal() != Core::ObjectID::Undefined();
	assert(hasParent == false);
	assert(HasChild(child.GetObjectID()) == false);

	child._parentID = GetObjectID();
	_childIDs.push_back(child.GetObjectID());
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
	Matrix rotMat = Matrix::MakeRotationMatrix(GetWorldRight(scale), GetWorldUp(scale), GetWorldForward(scale));

	return Vector3::FromRotationMatrix(rotMat);
}

const Quaternion Transform::FetchWorldQuaternion() const
{
	Vector3 scale = GetWorldScale();
	Matrix rotMat = Matrix::MakeRotationMatrix(GetWorldRight(scale), GetWorldUp(scale), GetWorldForward(scale));

	return Quaternion::FromRotationMatrix(rotMat);
}

bool Transform::HasChild(ObjectID id) const
{
	for (const auto childID : _childIDs)
	{
		if(childID.Literal() == id.Literal())
			return true;
	}

	return false;
}

void Transform::DeleteChild(ObjectID id)
{
	uint pos = 0;
	for (pos; pos < _childIDs.size() && _childIDs[pos] != id; ++pos);

	assert(pos == _childIDs.size());
	_childIDs.erase(_childIDs.begin() + pos);
}

void Transform::Update(TransformPool& pool)
{
	// this func must be run in root
	assert(_parentID.Literal() == ObjectID::Undefined());

	UpdateDirty(pool);
	ComputeWorldMatrix(pool);
}

void Transform::ClearDirty()
{
	_dirty = false;
}

void Transform::UpdateDirty(TransformPool& pool)
{
	for (uint id = _objectID.Literal(); id != ObjectID::Undefined(); id = _parentID.Literal())
		pool.Find(id)->_dirty |= _dirty;
}

void Transform::_ComputeWorldMatrix(TransformPool& pool)
{
	for (auto childID : _childIDs)
	{
		auto child = pool.Find(childID.Literal());
		assert(child);

		child->_worldMat = child->ComputeLocalMatrix() * _worldMat;
		child->_ComputeWorldMatrix(pool);
	}
}

void Transform::ComputeWorldMatrix(TransformPool& pool)
{
	// this func must be run in root
	assert(_parentID.Literal() == ObjectID::Undefined());

	if(_dirty == false)	return;
	_worldMat = ComputeLocalMatrix();

	_ComputeWorldMatrix(pool);
}
