#pragma once

#include "ObjectId.hpp"
#include "MeshManager.hpp"
#include "LightManager.h"
//#include "CameraManager.h"

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

	private:
		std::tuple<	Rendering::Manager::MeshManager,
					Rendering::Manager::LightManager
/*					Rendering::Manager::CameraManager*/	>	_componentMgrs;
	};
}