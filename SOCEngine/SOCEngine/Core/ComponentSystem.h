#pragma once

#include "ObjectID.hpp"
#include "MeshManager.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "ShadowManager.h"
#include <assert.h>

namespace Core
{
	class ComponentSystem final
	{
	private:
		using Mesh			= Rendering::Geometry::Mesh;
		using MainCamera	= Rendering::Camera::MainCamera;

		using DL			= Rendering::Light::DirectionalLight;
		using DLShadow		= Rendering::Shadow::DirectionalLightShadow;
		using PL			= Rendering::Light::PointLight;
		using PLShadow		= Rendering::Shadow::PointLightShadow;
		using SL			= Rendering::Light::SpotLight;
		using SLShadow		= Rendering::Shadow::SpotLightShadow;

	public:
		ComponentSystem() = default;

		DISALLOW_ASSIGN(ComponentSystem);
		DISALLOW_COPY_CONSTRUCTOR(ComponentSystem);

	public:
		void Initialize(Device::DirectX& dx);

		void UpdateBuffer(Device::DirectX& dx,
			const Core::TransformPool& transformPool,
			const Core::ObjectManager& objectManager,
			const Core::ObjectID::IndexHashMap& lightShaftIndexer);

		void Destroy();

	public:
#pragma region MultiTypeComponents
		template <class Component>
		Component& Add(ObjectID id)
		{
			auto& mgr = GetManager<Component>();
			return mgr.Acquire<Component>(id);
		}

		template <class Component>
		void Delete(ObjectID id)
		{
			auto& mgr = GetManager<Component>();
			mgr.Delete<Component>(id);
		}
		template <class Component>
		bool Has(ObjectID id) const
		{
			auto& mgr = GetManager<Component>();
			return mgr.Has<Component>(id);
		}
		template <class Component>
		auto Find(ObjectID id)
		{
			auto& mgr = GetManager<Component>();
			return mgr.Find<Component>(id);
		}
#pragma endregion Light, Shadow, Etc..

#pragma region Light
		template <> void Delete<DL>(ObjectID id)
		{
			auto& shadowMgr = GetManager<DL::ShadowType>();
			if (shadowMgr.Has<DL::ShadowType>(id))
				shadowMgr.Delete<DL::ShadowType>(id);

			auto& lightMgr = GetManager<DL>();
			lightMgr.Delete<DL>(id);
		}
		template <> void Delete<PL>(ObjectID id)
		{
			auto& shadowMgr = GetManager<PL::ShadowType>();
			if (shadowMgr.Has<PL::ShadowType>(id))
				shadowMgr.Delete<PL::ShadowType>(id);

			auto& lightMgr = GetManager<PL>();
			lightMgr.Delete<PL>(id);
		}
		template <> void Delete<SL>(ObjectID id)
		{
			auto& shadowMgr = GetManager<SL::ShadowType>();
			if (shadowMgr.Has<SL::ShadowType>(id))
				shadowMgr.Delete<SL::ShadowType>(id);

			auto& lightMgr = GetManager<SL>();
			lightMgr.Delete<SL>(id);
		}
#pragma endregion Light

#pragma region Shadow
		template <> DLShadow& Add<DLShadow>(ObjectID id)
		{
			auto& lightMgr = GetManager<DLShadow::LightType>();
			assert(lightMgr.Has<DLShadow::LightType>(id));

			auto& shadowMgr = GetManager<DLShadow>();
			return shadowMgr.Acquire<DLShadow>(id);
		}
		template <> PLShadow& Add<PLShadow>(ObjectID id)
		{
			auto& lightMgr = GetManager<PLShadow::LightType>();
			assert(lightMgr.Has<PLShadow::LightType>(id));

			auto& shadowMgr = GetManager<PLShadow>();
			return shadowMgr.Acquire<PLShadow>(id);
		}
		template <> SLShadow& Add<SLShadow>(ObjectID id)
		{
			auto& lightMgr = GetManager<SLShadow::LightType>();
			assert(lightMgr.Has<SLShadow::LightType>(id));

			auto& shadowMgr = GetManager<SLShadow>();
			return shadowMgr.Acquire<SLShadow>(id);
		}
#pragma endregion Shadow

#pragma region Mesh
		template <> Mesh& Add<Mesh>(ObjectID id)
		{
			auto& mgr = GetManager<Mesh>();
			return mgr.Acquire(id);
		}
		template <> void Delete<Mesh>(ObjectID id)
		{
			auto& mgr = GetManager<Mesh>();
			mgr.Delete(id);
		}
		template <> bool Has<Mesh>(ObjectID id) const
		{
			auto& mgr = GetManager<Mesh>();
			return mgr.Has(id);
		}
		template <> auto Find<Mesh>(ObjectID id)
		{
			auto& mgr = GetManager<Mesh>();
			return mgr.Find(id);
		}
#pragma endregion Mesh

#pragma region MainCamera
		template <> MainCamera& Add<MainCamera>(ObjectID id)
		{
			assert(!"MainCamera must use SetMainCamera");
		}
		template <> void Delete<MainCamera>(ObjectID id)
		{
			assert(!"Invalid Call");
		}
		template <> bool Has<MainCamera>(ObjectID id) const
		{
			auto& mgr = std::get<Rendering::Manager::CameraManager>(_componentMgrs);
			return mgr.GetMainCamera().GetObjectID() == id;
		}
		template <> auto Find<MainCamera>(ObjectID id)
		{
			assert(!"MainCamera must use GetMainCamera");
		}

		MainCamera& SetMainCamera(ObjectID id)
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
		const MainCamera& GetMainCamera() const
		{
			const auto& mgr = std::get<Rendering::Manager::CameraManager>(_componentMgrs);
			return mgr.GetMainCamera();
		}

#pragma endregion MainCamera

	public:
		template <class ManagerType> auto& GetManager_Direct()
		{
			return std::get<ManagerType>(_componentMgrs);
		}
		template <class ManagerType> const auto& GetManager_Direct() const
		{
			return std::get<ManagerType>(_componentMgrs);
		}

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
					Rendering::Manager::CameraManager,
					Rendering::Manager::ShadowManager >	_componentMgrs;

		Intersection::BoundBox	_sceneBoundBox;
	};
}