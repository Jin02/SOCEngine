#pragma once

#include "ObjectId.hpp"
#include "MeshManager.hpp"
#include "LightManager.h"
#include "CameraManager.h"

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
			auto& mgr = GetManager<Component>();
			return mgr.Acquire<Component>(id);
		}
		template <class Component>
		void Delete(ObjectId id)
		{
			auto& mgr = GetManager<Component>();
			mgr.Delete<Component>(id);
		}
		template <class Component>
		bool Has(ObjectId id) const
		{
			auto& mgr = GetManager<Component>();
			return mgr.Has<Component>(id);
		}
		template <class Component>
		auto Find(ObjectId id)
		{
			auto& mgr = GetManager<Component>();
			return mgr.Find<Component>(id);
		}

	private:
		template <class Component>
		auto& GetManager()
		{
			return std::get<Component::ManagerType>(_componentMgrs);
		}

	private:
		std::tuple<	Rendering::Manager::MeshManager,
					Rendering::Manager::LightManager
/*					Rendering::Manager::CameraManager*/	>	_componentMgrs;
	};
}