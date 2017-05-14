#pragma once

#include "ObjectId.hpp"
#include <memory>
#include <string>
#include <cassert>

#include "MainSystemHeader.h"

namespace Core
{
	class Object final
	{
	public:
		Object(ObjectId id, DefaultComponentSystem& dcs, TransformPool& tfp)
			: _id(id), _componentSystem(&dcs), _transformPool(&tfp)
		{
			_transformPool->Add(id);
		}

		~Object()
		{
			_transformPool->Delete(_id);
		}

		/* Child */
		void AddChild(Object& child)
		{
			ObjectId childId = child.GetId();
			auto found = _transformPool->GetIndexer().Has(childId);
			assert(found);

			_transformPool->Find(_id)->AddChild(childId);
		}

		bool HasChild(const Object& child) const
		{
			return _transformPool->Find(_id)->HasChild(child.GetId());
		}

		void DeleteChild(Object& child)
		{
			_transformPool->Find(_id)->DeleteChild(child.GetId());
		}

		auto FindChild(Object& child)
		{
			return HasChild(child) ? _transformPool->Find(child.GetId()) : nullptr;
		}

		auto GetChild(uint index)
		{
			ObjectId id = _transformPool->Find(_id)->GetChild(index);			
			return _transformPool->Find(id);
		}

		/* Component */
		template <class Component>
		Component& AddComponent()
		{
			return _componentSystem->Add<Component>(_id);
		}

		template <class Component>
		void DeleteComponet()
		{
			_componentSystem->Delete<Component>(_id);
		}

		template <class Component>
		bool HasComponent() const
		{
			return _componentSystem->Has<Component>(_id);
		}

	public:
		GET_SET_ACCESSOR(Name, const std::string&, _name);
		GET_CONST_ACCESSOR(Id, ObjectId, _id);

	private:
		ObjectId				_id;
		std::string				_name = "";

		DefaultComponentSystem*	_componentSystem;
		TransformPool*			_transformPool;
	};
}