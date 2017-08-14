#pragma once

#include "Common.h"
#include <string>

#include "ObjectId.hpp"
#include "ComponentSystem.hpp"

namespace Core
{
	class TransformPool;
	class ObjectManager;

	class Object final
	{
	public:
		Object(ObjectId id, ComponentSystem* compoSystem, TransformPool* tfPool, ObjectManager* objMgr);
		~Object();

		void AddChild(Object & child);
		bool HasChild(const Object & child) const;
		void DeleteChild(Object & child);
		auto FindChildUsingIndex(uint index);

		uint GetChildCount() const;

		/* Component */
		template <class Component>
		Component& AddComponent()
		{
			return _compoSystem->Add<Component>(_id);
		}
		template <class Component>
		void DeleteComponet()
		{
			_compoSystem->Delete<Component>(_id);
		}
		template <class Component>
		bool HasComponent() const
		{
			_compoSystem->Has<Component>(_id);
		}
		template <class Component>
		Component& GetComponent()
		{
			return _compoSystem->Find<Compoent>(_id);
		}

		// Object Clone() const; TODO: https://goo.gl/qVi3Yo

	public:
		GET_CONST_ACCESSOR(Name, auto&, _name);
		GET_CONST_ACCESSOR(ObjectId, auto, _id);

	private:
		std::string			_name;
		ObjectId			_id;

		TransformPool*		_tfPool;
		ComponentSystem*	_compoSystem;
		ObjectManager*		_objectMgr;
	};
}
