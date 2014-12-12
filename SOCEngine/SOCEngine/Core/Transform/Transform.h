#pragma once

#include "EngineMath.h"
#include "Intersection.h"
#include "Common.h"

namespace Core
{
	class Object;

	class Transform
	{
	private:
		Object		*_owner;

	protected:		
		Math::Vector3 _forward;
		Math::Vector3 _right;
		Math::Vector3 _up;

		Math::Vector3		_position;
		Math::Quaternion	_rotation;
		Math::Vector3		_scale;
		Math::Vector3		_eulerAngle;

		float _radius;

	public:
		Transform(Object* owner);
		~Transform(void);

	public:
		static void UpdateAxisToRotationMatrix(Math::Matrix& outMatrix, const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward);
		static void UpdateRotationMatrixToAxis(const Math::Matrix& rotationMatrix, Math::Vector3& outRight, Math::Vector3& outUp, Math::Vector3& outForward);

	public:
		void LookAt(Transform *target);
		void LookAt(const Math::Vector3& worldPosition);

		void Rotate(const Math::Vector3& eulerAngles);
		void Rotate(float x, float y, float z);

		void Translate(const Math::Vector3& translation);
		void TranslateWithUpVec(float units);
		void TranslateWithForwardVec(float units);
		void TranslateWithRightVec(float units);

		void Billboard(Math::Matrix& outMatrix, const Math::Matrix& camWorldMat);

	public:
		void UpdatePosition(const Math::Vector3& position);		
		void UpdateRotation(const Math::Quaternion& quaternion, bool updateAxis = true);				
		void UpdateEulerAngles(const Math::Vector3& euler, bool updateAxis = true);
		void UpdateScale(const Math::Vector3& scale);
		void UpdateDirection(const Math::Vector3& dir);

	public:
		GET_SET_ACCESSOR(Radius, float, _radius);
		GET_ACCESSOR(LocalPosition, const Math::Vector3&, _position);
		GET_ACCESSOR(LocalEulerAngle, const Math::Vector3&, _eulerAngle);
		GET_ACCESSOR(LocalScale, const Math::Vector3&, _scale);
		GET_ACCESSOR(LocalRotation, const Math::Quaternion&, _rotation);

		GET_ACCESSOR(Forward, const Math::Vector3&, _forward);
		GET_ACCESSOR(Up, const Math::Vector3&, _up);
		GET_ACCESSOR(Right, const Math::Vector3&, _right);

		void WorldMatrix(Math::Matrix& outMatrix);
		void WorldPosition(Math::Vector3& outPosition);
		void WorldEulerAngle(Math::Vector3& outEuler);
		void WorldScale(Math::Vector3& outScale);
		void WorldTransform(Transform& out);
	};
}