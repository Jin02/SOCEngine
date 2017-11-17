#pragma once

#include "Common.h"
#include <string>

#include "ObjectID.hpp"
#include "ComponentSystem.h"

#include "CoreConnector.h"

namespace Core
{
	class TransformPool;
	class ObjectManager;

	class Object final
	{
	public:
		Object(const std::string& name) : _name(name) {}
		static Object Create(ObjectID id, const std::string& name) { Object obj(name); obj.Initialize(id); return obj; }
		void Initialize(ObjectID id);
		void Destroy();

		void AddChild(Object& child);
		bool HasChild(const Object& child) const;
		void DeleteChild(Object& child);
		auto FindChildUsingIndex(uint index);

		uint GetChildCount() const;
		Transform& FetchTransform();

		void HierarchyUse(bool parentUse);
		void Use(bool use);

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
			return CoreConnector::SharedInstance()->GetComponentSystem()->Has<Component>(_id);
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

		GET_CONST_ACCESSOR(Use,			bool,	_use & _parentUse);

	private:
		std::string			_name = "";
		ObjectID			_id;

		bool				_parentUse	= true;
		bool				_use		= true;
	};
}
