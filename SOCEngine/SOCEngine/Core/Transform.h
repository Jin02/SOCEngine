#pragma once

#include "EngineMath.h"
#include "Common.h"
#include "ObjectId.hpp"
#include "VectorIndexer.hpp"
#include <memory>

namespace Core
{
	class TransformPool;

	class Transform
	{
	public:
		explicit Transform(ObjectId id) : _objectId(id) {}

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
		GET_CONST_ACCESSOR(ObjectId, ObjectId, _objectId);
		GET_CONST_ACCESSOR(ParentId, ObjectId, _parentId);

		void LookAtPos(const Math::Vector3& targetPos, const Math::Vector3* up = nullptr);
		void LookAtDir(const Math::Vector3& targetDir, const Math::Vector3* up = nullptr);
		
		using Childs = std::vector<ObjectId>;

		void		AddChild(Transform& child);
		ObjectId	GetChild(uint index)		{ return _childIds[index];	}
		ObjectId	GetChild(uint index) const	{ return _childIds[index]; }
		void		DeleteAllChilds()			{ _childIds.clear();		}
		uint		GetChildCount()	const		{ return _childIds.size();	}

		bool		HasChild(ObjectId id) const;
		void		DeleteChild(ObjectId id);

	private:
		const Math::Vector3 GetWorldForward(const Math::Vector3& scale)	const;
		const Math::Vector3	GetWorldRight(const Math::Vector3& scale) const;
		const Math::Vector3	GetWorldUp(const Math::Vector3& scale) const;

		const Math::Matrix& ComputeLocalMatrix();

		void _ComputeWorldMatrix(TransformPool& pool);
		void ComputeWorldMatrix(TransformPool& pool);

		void UpdateDirty(TransformPool& pool);

		inline void SetDirty()		{ _dirty = true; }
		inline void ResetDirty()	{ _dirty = false; }

	private:
		Math::Matrix		_worldMat;
		Math::Matrix		_localMat;

		Math::Vector3		_position		= Math::Vector3(0.0f, 0.0f, 0.0f);
		Math::Vector3		_eulerAngle		= Math::Vector3(0.0f, 0.0f, 0.0f);
		Math::Quaternion	_quaternion		= Math::Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
		Math::Vector3		_scale			= Math::Vector3(1.0f, 1.0f, 1.0f);

		Math::Vector3		_forward		= Math::Vector3(0.0f, 0.0f, 1.0f);
		Math::Vector3		_up				= Math::Vector3(0.0f, 1.0f, 0.0f);
		Math::Vector3		_right			= Math::Vector3(1.0f, 0.0f, 0.0f);

		ObjectId			_objectId;
		bool				_dirty			= true;

		ObjectId			_parentId;
		Childs				_childIds;
	};

	class TransformPool final : public Core::VectorHashMap<ObjectId::LiteralType, Transform>
	{
	public:
		using Parent = Core::VectorHashMap<ObjectId::LiteralType, Transform>;
		using Parent::Parent;
		using Parent::Add;

		DISALLOW_ASSIGN(TransformPool);

		auto& Add(ObjectId id)
		{
			Transform newTransform(id);
			return Parent::Add(id.Literal(), newTransform);
		}
	};
}
