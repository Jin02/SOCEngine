#pragma once

#include "EngineMath.h"
#include "Intersection.h"
#include "Common.h"

namespace Core
{
	class Transform
	{
	private:
		Transform *_root;
		Transform *_parent;

	protected:		
		Math::Vector3 _forward;
		Math::Vector3 _right;
		Math::Vector3 _up;

		Math::Vector3 _position;
		Math::Vector3 _localPosition;

		Math::Quaternion _rotation;

		Math::Vector3 _scale;
		Math::Vector3 _localScale;

		Math::Vector3 _eulerAngles;
		Math::Vector3 _localEulerAngle;

		Math::Matrix _matrix;

	private:
		float _radius;
		Intersection::AABB _bound;

	public:
		Transform(Transform* parent);
		~Transform(void);

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

	protected:
		void UpdateMatrix();

	public:
		void UpdateWorldTransform();

	public:
		void UpdatePosition(const Math::Vector3& position);		
		void UpdateRotation(const Math::Quaternion& quaternion);				
		void UpdateScale(const Math::Vector3& scale);
		void UpdateEulerAngles(const Math::Vector3& euler);
		void UpdateDirection(const Math::Vector3& dir);

	public:
		GET_SET_ACCESSOR(Bound, const Intersection::AABB&, _bound);
		GET_SET_ACCESSOR(Radius, const float, _radius);
		GET_ACCESSOR(Matrix, const Math::Matrix&, _matrix);
		GET_ACCESSOR(LocalPosition, const Math::Vector3&, _localPosition);
		GET_ACCESSOR(LocalEulerAngle, const Math::Vector3&, _localEulerAngle);
		GET_ACCESSOR(LocalScale, const Math::Vector3&, _localScale);
		GET_ACCESSOR(LocalRotation, const Math::Quaternion&, _rotation);

		GET_ACCESSOR(Forward, const Math::Vector3&, _forward);
		GET_ACCESSOR(Up, const Math::Vector3&, _up);
		GET_ACCESSOR(Right, const Math::Vector3&, _right);

		void WorldMatrix(Math::Matrix& outMatrix);
		void WorldPosition(Math::Vector3& outPosition);
		void WorldEulerAngle(Math::Vector3& outEuler);
		void WorldScale(Math::Vector3& outScale);
		float CalcRadius(Transform *child);

		//void GetWorldMatrix(SOC_Matrix *outMatrix);

		//Vector3 GetWorldPosition();
		//Vector3 GetLocalPosition();

		//Vector3 GetLocalEulerAngle();
		//Vector3 GetWorldEulerAngle();

		//Vector3 GetLocalScale();
		//Vector3 GetWorldScale();

		//SOC_Quaternion GetRotation();

		//Vector3 GetForward();
		//Vector3 GetRight();
		//Vector3 GetUp();

		//float GetRadius();
		//void SetRadius(float radius);

		//void SetBound(Intersection::AABB &bound);
		//Intersection::AABB GetBound();
	};
}