#pragma once

#include "ObjectId.hpp"
#include <memory>
#include <string>

namespace Core
{
	class Engine;
	class Object final
	{
	public:
		Object(ObjectId id, const std::weak_ptr<Engine>& engine)
			: _id(id), _engine(engine)
		{

		}

		template <class Component>
		Component& AddComponent()
		{
			assert(_engine.expired());
			return _engine.lock()->AddComponent<Component>(_id);
		}

		template <class Component>
		void DeleteComponet()
		{
			assert(_engine.expired());
			_engine.lock()->DeleteComponent<Component>(_id);
		}

		template <class Component>
		bool HasComponent() const
		{
			assert(_engine.expired());
			return _engine.lock()->HasComponent<Component>();
		}

	public:
		GET_SET_ACCESSOR(Name, const std::string&, _name);
		GET_CONST_ACCESSOR(Id, const ObjectId&, _id);

	private:
		ObjectId				_id;
		std::weak_ptr<Engine>	_engine;
		std::string				_name = "";
	};
}