#pragma once

#include "Common.h"
#include <string>

#include "ObjectID.hpp"
#include "ComponentSystem.hpp"

#include "CoreConnector.h"

namespace Core
{
	class TransformPool;
	class ObjectManager;

	class Object final
	{
	public:
		Object(ObjectID id);
		~Object();

		void AddChild(Object& child);
		bool HasChild(const Object& child) const;
		void DeleteChild(Object& child);
		auto FindChildUsingIndex(uint index);

		uint GetChildCount() const;

		/* Component */
		template <class Component>
		Component& AddComponent()
		{			
			return CoreConnector::SharedInstance()->GetComponentSystem()->Add<Component>(_id);
		}
		template <class Component>
		void DeleteComponet()
		{
			CoreConnector::SharedInstance()->GetComponentSystem()->Delete<Component>(_id);
		}
		template <class Component>
		bool HasComponent() const
		{
			CoreConnector::SharedInstance()->GetComponentSystem()->Has<Component>(_id);
		}
		template <class Component>
		Component& GetComponent()
		{
			return CoreConnector::SharedInstance()->GetComponentSystem()->Find<Compoent>(_id);
		}

		// Object Clone() const; TODO: https://goo.gl/qVi3Yo

	public:
		GET_CONST_ACCESSOR(Name,		auto&,	_name);
		GET_CONST_ACCESSOR(ObjectID,	auto,	_id);
		GET_SET_ACCESSOR(Use,			bool,	_use);

	private:
		std::string			_name;
		ObjectID			_id;

		bool				_use = true;
	};
}
