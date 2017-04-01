#pragma once

#include "EngineMath.h"
#include "Common.h"
#include "ObjectId.hpp"
#include "VectorIndexer.hpp"
#include <memory>

namespace Core
{
	class Transform
	{
	public:
		explicit Transform(ObjectId id);

		const Math::Vector3			GetWorldPosition()		const;
		const Math::Vector3			GetWorldScale()			const;
		const Math::Vector3			FetchWorldEulerAngle()	const;
		const Math::Quaternion		FetchWorldQuaternion()	const;

		inline const Math::Vector3	GetWorldForward()		const { return GetWorldForward(GetWorldScale()); }
		inline const Math::Vector3	GetWorldRight()			const { return GetWorldRight(GetWorldScale()); }
		inline const Math::Vector3	GetWorldUp()			const { return GetWorldUp(GetWorldScale()); }

		void UpdatePosition(const Math::Vector3& p);
		void UpdateScale(const Math::Vector3& s);
		void UpdateRight(const Math::Vector3& r);
		void UpdateUp(const Math::Vector3& u);
		void UpdateForward(const Math::Vector3& f);
		void UpdateEulerAngle(const Math::Vector3& e);
		void UpdateQuaternion(const Math::Quaternion& q);

		GET_CONST_ACCESSOR(Position, const Math::Vector3&, _position);
		GET_CONST_ACCESSOR(Scale, const Math::Vector3&, _scale);
		GET_CONST_ACCESSOR(Right, const Math::Vector3&, _right);
		GET_CONST_ACCESSOR(Up, const Math::Vector3&, _up);
		GET_CONST_ACCESSOR(Forward, const Math::Vector3&, _forward);
		GET_CONST_ACCESSOR(EularAngle, const Math::Vector3&, _eulerAngle);
		GET_CONST_ACCESSOR(Quaternion, const Math::Quaternion&, _quaternion);
		GET_CONST_ACCESSOR(LocalMatrix, const Math::Matrix&, _localMat);
		GET_CONST_ACCESSOR(WorldMatrix, const Math::Matrix&, _worldMat);

		GET_CONST_ACCESSOR(Dirty, bool, _dirty);
		GET_CONST_ACCESSOR(ObjectId, const ObjectId&, _objectId);

		void LookAtPos(const Math::Vector3& targetPos, const Math::Vector3* up = nullptr);
		void LookAtDir(const Math::Vector3& targetDir, const Math::Vector3* up = nullptr);

		using TransformPtr = std::shared_ptr<Transform>;
		using Childs = VectorHashMap<ObjectId::LiteralType, TransformPtr>;

		void		     AddChild(ObjectId key, TransformPtr child);
		TransformPtr	 FindChild(ObjectId id);
		TransformPtr	 GetChild(uint index);
		void		     DeleteChild(ObjectId id);
		void		     DeleteAllChilds();

	private:
		const Math::Vector3 GetWorldForward(const Math::Vector3& scale)	const;
		const Math::Vector3	GetWorldRight(const Math::Vector3& scale) const;
		const Math::Vector3	GetWorldUp(const Math::Vector3& scale) const;

		const Math::Matrix& ComputeLocalMatrix();

		void _ComputeWorldMatrix();
		void ComputeWorldMatrix();

		void SetDirty();
		inline void ResetDirty() { _dirty = false; }

	private:
		Math::Matrix		_worldMat;
		Math::Matrix		_localMat;

		Math::Vector3		_position;
		Math::Vector3		_eulerAngle;
		Math::Quaternion	_quaternion;
		Math::Vector3		_scale;

		Math::Vector3		_forward;
		Math::Vector3		_up;
		Math::Vector3		_right;

		const ObjectId		_objectId;
		bool				_dirty;

		TransformPtr		_parent;
		Childs				_childs;
	};

	class TransformPool final : public Core::VectorHashMap<ObjectId::LiteralType, Transform>
	{
	public:
		using Core::VectorHashMap<ObjectId::LiteralType, Transform>::VectorHashMap;
		DISALLOW_ASSIGN(TransformPool);
	};
}
