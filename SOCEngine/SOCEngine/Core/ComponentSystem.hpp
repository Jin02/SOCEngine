#pragma once

#include "ObjectId.hpp"
#include "MeshManager.hpp"
#include "LightManager.h"
#include "CameraManager.h"

namespace Core
{
	class ComponentSystem final
	{
	private:
		using Mesh = Rendering::Geometry::Mesh;
		using MainCamera = Rendering::Camera::MainCamera;

	public:
		ComponentSystem() = default;

		DISALLOW_ASSIGN(ComponentSystem);
		DISALLOW_COPY_CONSTRUCTOR(ComponentSystem);

#pragma region MultiTypeComponents
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
#pragma endregion Light,Etc..

#pragma region Mesh
		template <> Mesh& Add<Mesh>(ObjectId id)
		{
			auto& mgr = GetManager<Mesh>();
			return mgr.Acquire(id);
		}
		template <> void Delete<Mesh>(ObjectId id)
		{
			auto& mgr = GetManager<Mesh>();
			mgr.Delete(id);
		}
		template <> bool Has<Mesh>(ObjectId id) const
		{
			auto& mgr = GetManager<Mesh>();
			return mgr.Has(id);
		}
		template <> auto Find<Mesh>(ObjectId id)
		{
			auto& mgr = GetManager<Mesh>();
			return mgr.Find(id);
		}
#pragma endregion Mesh

#pragma region MainCamera
		template <> MainCamera& Add<MainCamera>(ObjectId id)
		{
			assert(!"MainCamera must use SetMainCamera");
		}
		template <> void Delete<MainCamera>(ObjectId id)
		{
			assert(!"Invalid Call");
		}
		template <> bool Has<MainCamera>(ObjectId id) const
		{
			return true;
		}
		template <> auto Find<MainCamera>(ObjectId id)
		{
			assert(!"MainCamera must use GetMainCamera");
		}

		MainCamera& SetMainCamera(ObjectId id)
		{
			auto& mgr = std::get<Rendering::Manager::CameraManager>(_componentMgrs);
			mgr.SetMainCamera(id);

			return mgr.GetMainCamera();
		}
		MainCamera& GetMainCamera()
		{
			auto& mgr = std::get<Rendering::Manager::CameraManager>(_componentMgrs);
			return mgr.GetMainCamera();
		}
#pragma endregion MainCamera

	private:
		template <class Component> auto& GetManager()
		{
			return std::get<Component::ManagerType>(_componentMgrs);
		}
		template <class Component> const auto& GetManager() const
		{
			return std::get<Component::ManagerType>(_componentMgrs);
		}

	private:
		std::tuple<	Rendering::Manager::MeshManager,
					Rendering::Manager::LightManager,
					Rendering::Manager::CameraManager	>	_componentMgrs;
	};
}