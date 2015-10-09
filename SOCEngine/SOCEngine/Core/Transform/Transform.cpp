#include "Transform.h"
#include "Object.h"

using namespace Math;

namespace Core
{
	Transform::Transform(Object* owner) : _owner(owner), _updateCounter(0)
	{
		_rotation	= Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		_scale		= Vector3::One();

		if(owner == nullptr)
			return;

		_forward = Vector3(0.0f, 0.0f, 1.0f);
		_right	 = Vector3(1.0f, 0.0f, 0.0f);
		_up	 	 = Vector3(0.0f, 1.0f, 0.0f);
	}

	Transform::~Transform(void)
	{

	}

	void Transform::LookAtWorld(const Vector3& targetPosition)
	{
		Vector3 worldPos;
		FetchWorldPosition(worldPos);

		Vector3 dir = (targetPosition - worldPos).Normalize();

		_forward = dir;
		_right = Vector3::Cross(Vector3::Up(), dir).Normalize();
		_up = Vector3::Cross(dir, _right).Normalize();

		Matrix rotationMatrix;
		UpdateAxisToRotationMatrix(rotationMatrix, _right, _up, _forward);
		Quaternion::RotationMatrix(_rotation, rotationMatrix);

		_eulerAngle = _rotation.ToEuler();
		_eulerAngle.y = Math::Common::Rad2Deg( _eulerAngle.y );
		_eulerAngle.x = Math::Common::Rad2Deg( _eulerAngle.x );
		_eulerAngle.z = Math::Common::Rad2Deg( _eulerAngle.z );

		Math::Common::EulerNormalize(_eulerAngle, _eulerAngle);

		++_updateCounter;
	}

	void Transform::LookAtWorld(Transform *target)
	{
		Vector3 targetPos;
		target->FetchWorldPosition(targetPos);

		LookAtWorld(targetPos);
	}

	void Transform::Rotate(const Vector3& eulerAngles)
	{
		_eulerAngle += eulerAngles;
		++_updateCounter;
	}

	void Transform::Rotate(float x, float y, float z)
	{
		_eulerAngle += Vector3(x, y, z);
		++_updateCounter;
	}

	void Transform::Translate(const Vector3& translation)
	{
		_position += translation;
		++_updateCounter;
	}

	void Transform::TranslateWithUpVec(float units)
	{
		_position += _up * units;
		++_updateCounter;
	}

	void Transform::TranslateWithForwardVec(float units)
	{
		_position += _forward * units;
		++_updateCounter;
	}

	void Transform::TranslateWithRightVec(float units)
	{
		_position += _right * units;
		++_updateCounter;
	}

	void Transform::UpdatePosition(const Math::Vector3& position)
	{
		_position = position;
		++_updateCounter;
	}

	void Transform::UpdateRotation(const Math::Quaternion& quaternion, bool updateAxis)
	{
		_rotation = quaternion; 

		Vector3 euler;
		Math::Quaternion::ToEuler(euler, _rotation);

		_eulerAngle.x = Math::Common::Rad2Deg( euler.x );
		_eulerAngle.y = Math::Common::Rad2Deg( euler.y );
		_eulerAngle.z = Math::Common::Rad2Deg( euler.z );

		if(updateAxis)
		{
			Matrix rotationMatrix;
			Matrix::RotationQuaternion(rotationMatrix, _rotation);
			UpdateRotationMatrixToAxis(rotationMatrix, _right, _up, _forward);
		}

		++_updateCounter;
	}

	void Transform::UpdateEulerAngles(const Vector3& euler, bool updateAxis)
	{
		Math::Common::EulerNormalize(_eulerAngle, euler);

		Vector3 re;
		re.x = Math::Common::Deg2Rad( _eulerAngle.x );
		re.y = Math::Common::Deg2Rad( _eulerAngle.y );
		re.z = Math::Common::Deg2Rad( _eulerAngle.z );

		Quaternion::RotationYawPitchRoll(_rotation, re.y, re.x, re.z);

		if(updateAxis)
		{
			Matrix rotationMatrix;
			Matrix::RotationQuaternion(rotationMatrix, _rotation);
			UpdateRotationMatrixToAxis(rotationMatrix, _right, _up, _forward);
		}

		++_updateCounter;
	}

	void Transform::UpdateScale(const Vector3& scale)
	{
		_scale = scale;
		++_updateCounter;
	}

	void Transform::UpdateDirection(const Math::Vector3& dir)
	{
		LookAtWorld(_position + dir);
	}
	
	void Transform::Billboard(const Transform& camTransform)
	{
		Matrix camRotMat;
		UpdateAxisToRotationMatrix(camRotMat, camTransform._right, camTransform._up, camTransform._forward);

		Matrix billboardMat;
		Matrix::Inverse(billboardMat, camRotMat);

		Vector3 right, up, forward;
		UpdateRotationMatrixToAxis(billboardMat, right, up, forward);

		Vector3 normRight	= Vector3::Normalize(right);
		Vector3 normUp		= Vector3::Normalize(up);
		Vector3 normForward	= Vector3::Normalize(forward);

		Vector3 scale(right.x / normRight.x, up.y / normUp.y, forward.z / normForward.z);
		_scale = scale;

		_right		= normRight;
		_up			= normUp;
		_forward	= normForward;

		Quaternion::RotationMatrix(_rotation, billboardMat);
		_eulerAngle = _rotation.ToEuler();
		//_position은 그대로

		++_updateCounter;
	}

	void Transform::UpdateAxisToRotationMatrix(Math::Matrix& outMatrix, const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward)
	{
		outMatrix._m[0][0] = right.x;
		outMatrix._m[0][1] = up.x;
		outMatrix._m[0][2] = forward.x;
		outMatrix._m[0][3] = 0.0f;

		outMatrix._m[1][0] = right.y;
		outMatrix._m[1][1] = up.y;
		outMatrix._m[1][2] = forward.y;
		outMatrix._m[1][3] = 0.0f;

		outMatrix._m[2][0] = right.z;
		outMatrix._m[2][1] = up.z;
		outMatrix._m[2][2] = forward.z;
		outMatrix._m[2][3] = 0.0f;

		outMatrix._m[3][0] = 0;
		outMatrix._m[3][1] = 0;
		outMatrix._m[3][2] = 0;
		outMatrix._m[3][3] = 1.0f;
	}

	void Transform::UpdateRotationMatrixToAxis(const Math::Matrix& rotationMatrix, Math::Vector3& outRight, Math::Vector3& outUp, Math::Vector3& outForward)
	{
		outRight	= Vector3(rotationMatrix._m[0][0], rotationMatrix._m[1][0], rotationMatrix._m[2][0]);
		outUp 		= Vector3(rotationMatrix._m[0][1], rotationMatrix._m[1][1], rotationMatrix._m[2][1]);
		outForward	= Vector3(rotationMatrix._m[0][2], rotationMatrix._m[1][2], rotationMatrix._m[2][2]);
	}

	void Transform::UpdateTransform(const Transform& transform)
	{
		_forward	= transform._forward;
		_right		= transform._right;
		_up			= transform._up;
		_position	= transform._position;
		_rotation	= transform._rotation;
		_scale		= transform._scale;
		_eulerAngle	= transform._eulerAngle;

		++_updateCounter;
	}

	void Transform::FetchLocalMatrix(Math::Matrix& outMatrix) const
	{
		Matrix& localMat = outMatrix;
		Matrix::Identity(localMat);

		localMat._11 = _right.x;
		localMat._12 = _up.x;
		localMat._13 = _forward.x;

		localMat._21 = _right.y;
		localMat._22 = _up.y;
		localMat._23 = _forward.y;

		localMat._31 = _right.z;
		localMat._32 = _up.z;
		localMat._33 = _forward.z;

		localMat._11 *= _scale.x;
		localMat._22 *= _scale.y;
		localMat._33 *= _scale.z;

		localMat._41 = _position.x;
		localMat._42 = _position.y;
		localMat._43 = _position.z;
	}

	void Transform::FetchWorldMatrix(Math::Matrix& outMatrix) const
	{
		Matrix& worldMat = outMatrix;
		Matrix::Identity(worldMat);

		for(const Object* obj = _owner; obj != nullptr; obj = obj->GetParent())
		{
			Matrix localMat;
			obj->GetTransform()->FetchLocalMatrix(localMat);

			worldMat *= localMat;
		}
	}

	void Transform::FetchWorldPosition(Math::Vector3& outPosition) const
	{
		Matrix worldMat;
		FetchWorldMatrix(worldMat);

		outPosition.x = worldMat._41;
		outPosition.y = worldMat._42;
		outPosition.z = worldMat._43;
	}

	void Transform::FetchWorldEulerAngle(Math::Vector3& outEuler) const
	{
		Matrix worldMat;
		FetchWorldMatrix(worldMat);

		Vector3 worldRight	= Vector3(worldMat._11, worldMat._21, worldMat._31);
		Vector3 normRight	= worldRight.Normalize();
		worldMat._11 = normRight.x;	worldMat._21 = normRight.y;	worldMat._31 = normRight.z;

		Vector3 worldUp	= Vector3(worldMat._12, worldMat._22, worldMat._32);
		Vector3 normUp	= worldUp.Normalize();
		worldMat._12 = normUp.x;	worldMat._22 = normUp.y;	worldMat._32 = normUp.z;

		Vector3 worldForward = Vector3(worldMat._13, worldMat._23, worldMat._33);
		Vector3 normForward	 = worldForward.Normalize();
		worldMat._13 = normForward.x;	worldMat._23 = normForward.y;	worldMat._33 = normForward.z;

		worldMat._41 = worldMat._42 = worldMat._43 = 0.0f;

		Quaternion rotation;
		Quaternion::RotationMatrix(rotation, worldMat);

		outEuler = rotation.ToEuler();
	}

	void Transform::FetchWorldScale(Math::Vector3& outScale) const
	{
		Matrix worldMat;
		FetchWorldMatrix(worldMat);

		Vector3 worldRight	= Vector3(worldMat._11, worldMat._21, worldMat._31);
		Vector3 normRight	= worldRight.Normalize();
		outScale.x = worldRight.x / normRight.x;

		Vector3 worldUp	= Vector3(worldMat._12, worldMat._22, worldMat._32);
		Vector3 normUp	= worldUp.Normalize();
		outScale.y = worldUp.y / normUp.y;

		Vector3 worldForward = Vector3(worldMat._13, worldMat._23, worldMat._33);
		Vector3 normForward	 = worldForward.Normalize();
		outScale.z = worldForward.z / normForward.z;
	}

	void Transform::FetchWorldTransform(Transform& out) const
	{
		Matrix worldMat;
		FetchWorldMatrix(worldMat);

		Vector3& scale		= out._scale;
		Vector3& euler		= out._eulerAngle;
		Vector3& position	= out._position;
		Vector3& right		= out._right;
		Vector3& up			= out._up;
		Vector3& forward	= out._forward;
		Quaternion& rotation = out._rotation;

		Vector3 worldRight	= Vector3(worldMat._11, worldMat._21, worldMat._31);
		right = worldRight.Normalize();
		scale.x = worldRight.x / right.x;

		Vector3 worldUp	= Vector3(worldMat._12, worldMat._22, worldMat._32);
		up = worldUp.Normalize();
		scale.y = worldUp.y / up.y;

		Vector3 worldForward = Vector3(worldMat._13, worldMat._23, worldMat._33);
		forward = worldForward.Normalize();
		scale.z = worldForward.z / forward.z;

		position.x = worldMat._41;
		position.y = worldMat._42;
		position.z = worldMat._43;

		Matrix rotMat; Matrix::Identity(rotMat);
		rotMat._11 = right.x;	rotMat._21 = right.y;	rotMat._31 = right.z;
		rotMat._12 = up.x;		rotMat._22 = up.y;		rotMat._32 = up.z;
		rotMat._13 = forward.x;	rotMat._23 = forward.y;	rotMat._33 = forward.z;

		Quaternion::RotationMatrix(rotation, worldMat);
		euler = rotation.ToEuler();
	}
}