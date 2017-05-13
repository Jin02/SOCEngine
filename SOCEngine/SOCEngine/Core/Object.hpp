#pragma once

#include "ObjectId.hpp"
#include <memory>
#include <string>
#include "Engine.h"

namespace Core
{
	class Object final
	{
	public:
		Object(ObjectId id, Engine* engine)
			: _id(id), _engine(engine)
		{

		}

		/* Child */
		void AddChild(Object& child)
		{
		}

		void HasChild(Object& child)
		{

		}

		void DeleteChild(Object& child)
		{

		}

		void FindChild(Object& child)
		{

		}

		void GetChild(uint index)
		{

		}


		/* Component */
		template <class Component>
		Component& AddComponent()
		{
			return _engine->GetComponentSystem().Add<Component>(_id);
		}

		template <class Component>
		void DeleteComponet()
		{
			_engine->GetComponentSystem().Delete<Component>(_id);
		}

		template <class Component>
		bool HasComponent() const
		{
			return _engine->GetComponentSystem().Has<Component>(_id);
		}

	public:
		GET_SET_ACCESSOR(Name, const std::string&, _name);
		GET_CONST_ACCESSOR(Id, const ObjectId&, _id);

	private:
		ObjectId				_id;
		std::string				_name = "";
		Engine*					_engine;
	};
}