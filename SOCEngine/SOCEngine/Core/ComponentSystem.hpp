#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectId.hpp"
#include <memory>
#include <tuple>

namespace Core
{
	template <class... Components>
	class ComponentSystem final
	{
	public:
		ComponentSystem() = default;

		DISALLOW_ASSIGN(ComponentSystem);
		DISALLOW_COPY_CONSTRUCTOR(ComponentSystem);

		template <class Component>
		Component& Add(ObjectId id)
		{
			auto& components = std::get<ComponentPool<Component>>(_components);
			components.Add(id.Literal(), Component(id));
			return components.Get( components.GetSize() - 1);
		}
		template <class Component>
		void Delete(ObjectId id)
		{
			auto& components = std::get<ComponentPool<Component>>(_components);
			components.Delete(id.Literal());
		}
		template <class Component>
		bool Has(ObjectId id) const
		{
			auto& components = std::get<ComponentPool<Component>>(_components);
			return components.GetIndexer().Has(id.Literal());
		}
		template <class Component>
		auto Find(ObjectId id)
		{
			auto& components = std::get<ComponentPool<Component>>(_components);
			return components.Find(id.Literal());
		}

	private:
		template <class ComponentType>
		using ComponentPool = VectorHashMap<ObjectId::LiteralType, ComponentType>;

		std::tuple<ComponentPool<Components>...>                    _components;
	};
}