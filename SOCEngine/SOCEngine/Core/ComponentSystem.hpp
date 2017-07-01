#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectId.hpp"
#include <memory>
#include <tuple>

#include "Transform.h"
#include "MeshManager.hpp"

namespace Core
{
	class ComponentSystem final
	{
	public:
		ComponentSystem() = default;

		DISALLOW_ASSIGN(ComponentSystem);
		DISALLOW_COPY_CONSTRUCTOR(ComponentSystem);

		template <class Component>
		Component& Add(ObjectId id)
		{
		}
		template <class Component>
		void Delete(ObjectId id)
		{
		}
		template <class Component>
		bool Has(ObjectId id) const
		{
		}
		template <class Component>
		auto Find(ObjectId id)
		{
		}

	public:
		GET_ACCESSOR(TransformPool, auto&, _transformPool);

	private:
		Core::TransformPool									_transformPool;
		Rendering::Manager::MeshManager						_meshManager;
	};
}