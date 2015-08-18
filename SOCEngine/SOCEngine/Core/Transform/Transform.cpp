#include "Transform.h"
#include "Object.h"

using namespace Math;

namespace Core
{
	Transform::Transform(Object* owner) : _owner(owner), _updateCounter(0)
	{
		_rotation	= Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		_scale		= Vector3::One();
		_radius		= 0.0f;

		if(owner == nullptr)
			return;

		if(owner->GetParent())
		{
			Transform* parentTransform = owner->GetParent()->GetTransform();

			_forward = parentTransform->_forward;
			_right	 = parentTransform->_right;
			_up	 	 = parentTransform->_up;
		}
		else
		{
			_forward = Vector3(0.0f, 0.0f, 1.0f);
			_right	 = Vector3(1.0f, 0.0f, 0.0f);
			_up	 	 = Vector3(0.0f, 1.0f, 0.0f);
		}
	}

	Transform::~Transform(void)
	{

	}

	void Transform::LookAt(const Vector3& targetPosition)
	{
		Vector3 worldPos;
		FetchWorldPosition(worldPos);

		Vector3 dir = targetPosition - worldPos;
		dir.Normalize();

		_forward = dir;
		_right = Vector3::Cross(Vector3::Up(), dir);
		_up = Vector3::Cross(dir, _right);
		_right.Normalize();
		_up.Normalize();

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

	void Transform::LookAt(Transform *target)
	{
		Vector3 targetPos;
		target->FetchWorldPosition(targetPos);

		LookAt(targetPos);
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

	void Transform::FetchWorldMatrix(Math::Matrix& outMatrix) const
	{
		Transform tf(nullptr);
		FetchWorldTransform(tf);

		Matrix::RotationQuaternion(outMatrix, tf._rotation);
		outMatrix._41 = tf._position.x;
		outMatrix._42 = tf._position.y;
		outMatrix._43 = tf._position.z;
		outMatrix._44 = 1.0f;

		outMatrix._11 *= tf._scale.x;
		outMatrix._22 *= tf._scale.y;
		outMatrix._33 *= tf._scale.z;
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
		Vector3 worldPos;
		FetchWorldPosition(worldPos);

		Vector3 p = worldPos + dir;
		LookAt(p);
	}

	void Transform::FetchWorldTransform(Transform& out) const
	{
		Vector3 p(0, 0, 0), s(1, 1, 1), e(0, 0, 0);
		Quaternion q;
		
		for(const Object* obj = _owner; obj != nullptr; obj = obj->GetParent())
		{
			Transform* tf = obj->GetTransform();

			p += tf->_position;

			s.x *= tf->_scale.x;
			s.y *= tf->_scale.y;
			s.z *= tf->_scale.z;

			e += _eulerAngle;
		}

		out._position = p;
		Math::Common::EulerNormalize(out._eulerAngle, e);
		out._scale = s;

		float yaw = Math::Common::Deg2Rad(out._eulerAngle.y);
		float pitch = Math::Common::Deg2Rad(out._eulerAngle.x);
		float roll = Math::Common::Deg2Rad(out._eulerAngle.z);

		Quaternion::RotationYawPitchRoll(out._rotation, yaw, pitch, roll);
	}

	void Transform::FetchWorldPosition(Math::Vector3& outPosition) const
	{
		Vector3 p(0, 0, 0);

		for(const Object* obj = _owner; obj != nullptr; obj = obj->GetParent())
		{
			Transform* tf = obj->GetTransform();
			p += tf->_position;
		}

		outPosition = p;
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

	void Transform::FetchWorldEulerAngle(Math::Vector3& outEuler) const
	{
		Vector3 p(0, 0, 0);

		for(const Object* obj = _owner; obj != nullptr; obj = obj->GetParent())
		{
			Transform* tf = obj->GetTransform();
			p += tf->_eulerAngle;
		}

		outEuler = p;
	}

	void Transform::FetchWorldScale(Math::Vector3& outScale) const
	{
		Vector3 p(1, 1, 1);

		for(const Object* obj = _owner; obj != nullptr; obj = obj->GetParent())
		{
			Transform* tf = obj->GetTransform();

			p.x *= tf->_scale.x;
			p.y *= tf->_scale.y;
			p.z *= tf->_scale.z;
		}

		outScale = p;
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
		_radius		= transform._radius;

		++_updateCounter;
	}
}