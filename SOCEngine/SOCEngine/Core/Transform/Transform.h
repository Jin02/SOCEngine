#pragma once

#include "EngineMath.h"
#include "Intersection.h"
#include "Common.h"

#include "VectorMap.h"

namespace Core
{
	class Object;

	class Transform
	{
	private:
		Object*											_owner;
		Transform*										_parent;
		Structure::VectorMap<std::string, Transform*>	_childs;

	protected:		
		Math::Vector3									_forward;
		Math::Vector3									_right;
		Math::Vector3									_up;

		Math::Vector3									_position;
		Math::Quaternion								_rotation;
		Math::Vector3									_scale;
		Math::Vector3									_eulerAngle;

		uint											_updateCounter;
		Math::Matrix									_worldMat;

	public:
		Transform(Object* owner);
		~Transform(void);

	private:
		void		_UpdateWorldMatrix();

	public:
		void		AddChild(const std::string& key, Transform* child);
		Transform*	FindChild(const std::string& key);
		Transform*	GetChild(uint index);
		void		DeleteChild(const std::string& key);
		void		DeleteAllChilds();
		void		UpdateWorldMatrix();

	public:
		static void FetchMatrixFromAxises(Math::Matrix& outMatrix, const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward);
		static void FetchAxisesFromRotationMatrix(const Math::Matrix& rotationMatrix, Math::Vector3& outRight, Math::Vector3& outUp, Math::Vector3& outForward);

	public:
		void LookAtWorld(const Transform *target, const Math::Vector3* up = nullptr);
		void LookAtWorld(const Math::Vector3& worldPosition, const Math::Vector3* up = nullptr);

		void Rotate(const Math::Vector3& eulerAngles);
		void Rotate(float x, float y, float z);

		void Translate(const Math::Vector3& translation);
		void TranslateWithUpVec(float units);
		void TranslateWithForwardVec(float units);
		void TranslateWithRightVec(float units);

		void Billboard(const Transform& camTransform);

	public:
		void UpdatePosition(const Math::Vector3& position);		
		void UpdateRotation(const Math::Quaternion& quaternion);				
		void UpdateEulerAngles(const Math::Vector3& euler);
		void UpdateScale(const Math::Vector3& scale);
		void UpdateDirection(const Math::Vector3& dir);
		void UpdateTransform(const Transform& transform);

	public:
		GET_ACCESSOR(LocalPosition,		const Math::Vector3&,		_position);
		GET_ACCESSOR(LocalEulerAngle,	const Math::Vector3&,		_eulerAngle);
		GET_ACCESSOR(LocalScale,		const Math::Vector3&,		_scale);
		GET_ACCESSOR(LocalRotation,		const Math::Quaternion&,	_rotation);

		GET_ACCESSOR(Forward,			const Math::Vector3&,		_forward);
		GET_ACCESSOR(Up,				const Math::Vector3&,		_up);
		GET_ACCESSOR(Right,				const Math::Vector3&,		_right);

		GET_ACCESSOR(WorldMatrix,		const Math::Matrix&,		_worldMat);

		void FetchLocalMatrix(Math::Matrix& outMatrix) const;
		void FetchWorldMatrix(Math::Matrix& outMatrix) const;
		void FetchWorldPosition(Math::Vector3& outPosition) const;
		void FetchWorldTransform(Transform& out) const;
		void FetchWorldScale(Math::Vector3& outScale) const;
		static void FetchWorldScale(Math::Vector3& outScale, const Math::Matrix& worldMat);

		GET_ACCESSOR(UpdateCounter, uint, _updateCounter);
		inline bool IsUpdated(uint counter) const { return _updateCounter != counter; }
		inline void AddUpdateCounter(uint v = 1) { _updateCounter+= v; };
	};
}