#pragma once

#include "EngineMath.h"
#include "Common.h"
#include "ObjectID.hpp"
#include "VectorIndexer.hpp"
#include <memory>

namespace Core
{
	class TransformPool;

	class Transform
	{
	public:
		explicit Transform(ObjectID id) : _objectID(id) {}

		const Math::Vector3			GetWorldPosition()		const;
		const Math::Vector3			GetWorldScale()			const;
		const Math::Vector3			FetchWorldEulerAngle()	const;
		const Math::Quaternion		FetchWorldQuaternion()	const;

		inline const Math::Vector3	GetWorldForward()		const { return GetWorldForward(GetWorldScale()); }
		inline const Math::Vector3	GetWorldRight()			const { return GetWorldRight(GetWorldScale()); }
		inline const Math::Vector3	GetWorldUp()			const { return GetWorldUp(GetWorldScale()); }

		void UpdateRight(const Math::Vector3& r);
		void UpdateUp(const Math::Vector3& u);
		void UpdateForward(const Math::Vector3& f);
		void UpdateEulerAngle(const Math::Vector3& e);
		void UpdateQuaternion(const Math::Quaternion& q);

		SET_ACCESSOR_DIRTY(LocalPosition,	const Math::Vector3&,		_position);
		SET_ACCESSOR_DIRTY(LocalScale,		const Math::Vector3&,		_scale);

		GET_CONST_ACCESSOR(LocalPosition,	const Math::Vector3&,		_position);
		GET_CONST_ACCESSOR(LocalScale,		const Math::Vector3&,		_scale);
		GET_CONST_ACCESSOR(LocalRight,		const Math::Vector3&,		_right);
		GET_CONST_ACCESSOR(LocalUp,			const Math::Vector3&,		_up);
		GET_CONST_ACCESSOR(LocalForward,	const Math::Vector3&,		_forward);
		GET_CONST_ACCESSOR(LocalEularAngle,	const Math::Vector3&,		_eulerAngle);
		GET_CONST_ACCESSOR(LocalQuaternion,	const Math::Quaternion&,	_quaternion);
		GET_CONST_ACCESSOR(LocalMatrix,		const Math::Matrix&,		_localMat);
		GET_CONST_ACCESSOR(WorldMatrix,		const Math::Matrix&,		_worldMat);

		GET_CONST_ACCESSOR(Dirty,			bool,						_dirty);
		GET_CONST_ACCESSOR(ObjectID,		ObjectID,					_objectID);
		GET_CONST_ACCESSOR(ParentID,		ObjectID,					_parentID);

		void LookAtWorldDir(const Math::Vector3& targetDir, const Math::Vector3* up = nullptr);

		using Childs = std::vector<ObjectID>;

		void		AddChild(Transform& child);

		ObjectID	GetChild(uint index)		{ return _childIDs[index];	}
		ObjectID	GetChild(uint index) const	{ return _childIDs[index];	}
		void		DeleteAllChilds()			{ _childIDs.clear();		}
		uint		GetChildCount()	const		{ return _childIDs.size();	}

		bool		HasChild(ObjectID id) const;
		void		DeleteChild(ObjectID id);
		void		UpdateWorldMatrix(TransformPool& pool);

		void		ClearDirty();

	private:
		const Math::Vector3 GetWorldForward(const Math::Vector3& scale)	const;
		const Math::Vector3	GetWorldRight(const Math::Vector3& scale) const;
		const Math::Vector3	GetWorldUp(const Math::Vector3& scale) const;

		const Math::Matrix& ComputeLocalMatrix();

		void _ComputeWorldMatrixWithDirty(TransformPool& pool, bool parentDirty);

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

		ObjectID			_objectID;
		bool				_dirty			= true;

		Childs				_childIDs;

		ObjectID			_parentID;
	};

	class TransformPool final : public Core::VectorHashMap<ObjectID::LiteralType, Transform>
	{
	public:
		using Parent = Core::VectorHashMap<ObjectID::LiteralType, Transform>;
		using Parent::Parent;
		using Parent::Add;

		TransformPool() = default;
		DISALLOW_ASSIGN_COPY(TransformPool);

		auto& Add(ObjectID id)
		{
			Transform newTransform(id);
			return Parent::Add(id.Literal(), newTransform);
		}
	};
}
