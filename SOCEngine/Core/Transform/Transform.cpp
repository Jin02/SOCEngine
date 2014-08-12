#include "Transform.h"

using namespace Math;

namespace Core
{
	Transform::Transform(Transform *parent)
	{
		_rotation		= Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		_scale			= Vector3::One();

		_parent = parent;
		if(parent)
		{
			_root = parent->_root;

			_forward = parent->_forward;
			_right	 = parent->_right;
			_up	 	 = parent->_up;
		}
		else
		{
			_root = this;

			_forward = Vector3(0.0f, 0.0f, 1.0f);
			_right	 = Vector3(1.0f, 0.0f, 0.0f);
			_up	 	 = Vector3(0.0f, 1.0f, 0.0f);
		}

		_radius = 0.0f;
	}

	Transform::~Transform(void)
	{

	}

	void Transform::LookAt(const Vector3& targetPosition)
	{
		Vector3 worldPos;
		WorldPosition(worldPos);

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
	}

	void Transform::LookAt(Transform *target)
	{
		Vector3 targetPos;
		target->WorldPosition(targetPos);

		LookAt(targetPos);
	}

	void Transform::Rotate(const Vector3& eulerAngles)
	{
		_eulerAngle += eulerAngles;
	}

	void Transform::Rotate(float x, float y, float z)
	{
		_eulerAngle += Vector3(x, y, z);
	}

	void Transform::Translate(const Vector3& translation)
	{
		_position += translation;
	}

	void Transform::TranslateWithUpVec(float units)
	{
		_position += _up * units;
	}

	void Transform::TranslateWithForwardVec(float units)
	{
		_position += _forward * units;
	}

	void Transform::TranslateWithRightVec(float units)
	{
		_position += _right * units;
	}

	void Transform::WorldMatrix(Math::Matrix& outMatrix)
	{
		Transform tf(nullptr);
		WorldTransform(tf);

		Matrix::RotationQuaternion(outMatrix, tf._rotation);
		outMatrix._14 = tf._position.x;
		outMatrix._24 = tf._position.y;
		outMatrix._34 = tf._position.z;
		outMatrix._44 = 1.0f;

		outMatrix._11 *= tf._scale.x;
		outMatrix._22 *= tf._scale.y;
		outMatrix._33 *= tf._scale.z;
	}

	void Transform::UpdatePosition(const Math::Vector3& position)
	{
		_position = position;
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
	}

	void Transform::UpdateScale(const Vector3& scale)
	{
		_scale = scale;
	}

	void Transform::UpdateDirection(const Math::Vector3& dir)
	{
		Vector3 worldPos;
		WorldPosition(worldPos);

		Vector3 p = worldPos + dir;
		LookAt(p);
	}

	void Transform::WorldTransform(Transform& out)
	{
		Vector3 p(0, 0, 0), s(1, 1, 1), e(0, 0, 0);
		Quaternion q;

		for(Transform *o = this; o != NULL; o = o->_parent)
		{
			p += o->_position;

			s.x *= o->_scale.x;
			s.y *= o->_scale.y;
			s.z *= o->_scale.z;

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

	void Transform::WorldPosition(Math::Vector3& outPosition)
	{
		Vector3 p(0, 0, 0);

		for(Transform *o = this; o != NULL; o = o->_parent)
			p += o->_position;

		outPosition = p;
	}

	void Transform::Billboard(Math::Matrix& outMatrix, const Math::Matrix& camWorldMat)
	{
		Matrix mat = camWorldMat;

		mat._11 = camWorldMat._11;
		mat._13 = camWorldMat._13;
		mat._31 = camWorldMat._31;
		mat._33 = camWorldMat._33;

		Matrix::Inverse(outMatrix, mat);
	}

	void Transform::WorldEulerAngle(Math::Vector3& outEuler)
	{
		Vector3 p(0, 0, 0);

		for(Transform *o = this; o != NULL; o = o->_parent)
			p += o->_eulerAngle;

		outEuler = p;
	}

	void Transform::WorldScale(Math::Vector3& outScale)
	{
		Vector3 p(1, 1, 1);

		for(Transform *o = this; o != NULL; o = o->_parent)
		{
			p.x *= o->_scale.x;
			p.y *= o->_scale.y;
			p.z *= o->_scale.z;
		}

		outScale = p;
	}

	void Transform::UpdateAxisToRotationMatrix(Math::Matrix& outMatrix, const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward)
	{
		outMatrix._m[0][0] = right.x;
		outMatrix._m[0][1] = right.y;
		outMatrix._m[0][2] = right.z;
		outMatrix._m[0][3] = 0.0f;

		outMatrix._m[1][0] = up.x;
		outMatrix._m[1][1] = up.y;
		outMatrix._m[1][2] = up.z;
		outMatrix._m[1][3] = 0.0f;

		outMatrix._m[2][0] = forward.x;
		outMatrix._m[2][1] = forward.y;
		outMatrix._m[2][2] = forward.z;
		outMatrix._m[2][3] = 0.0f;

		outMatrix._m[3][0] = 0;
		outMatrix._m[3][1] = 0;
		outMatrix._m[3][2] = 0;
		outMatrix._m[3][3] = 1.0f;
	}

	void Transform::UpdateRotationMatrixToAxis(const Math::Matrix& rotationMatrix, Math::Vector3& outRight, Math::Vector3& outUp, Math::Vector3& outForward)
	{
		outRight	= Vector3(rotationMatrix._m[0][0], rotationMatrix._m[0][1], rotationMatrix._m[0][2]);
		outUp 		= Vector3(rotationMatrix._m[1][0], rotationMatrix._m[1][1], rotationMatrix._m[1][2]);
		outForward	= Vector3(rotationMatrix._m[2][0], rotationMatrix._m[2][1], rotationMatrix._m[2][2]);
	}
}