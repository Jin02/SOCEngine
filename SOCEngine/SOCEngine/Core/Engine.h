#pragma once

#include "Common.h"
#include "IScene.h"
#include "MaterialManager.hpp"
#include "BufferManager.h"
#include "ObjectManager.h"
#include "MeshManager.hpp"
#include "ShaderManager.h"
#include "Texture2DManager.h"
#include "Object.h"
#include "ComponentSystem.hpp"
#include <memory>
#include <chrono>

namespace Device
{
	class DirectX;
}

namespace Core
{
	class Engine final
	{
	public:
		Engine(Device::DirectX& dx);
		Engine(Device::DirectX& dx, IScene* scene);

		DISALLOW_ASSIGN(Engine);
		DISALLOW_COPY_CONSTRUCTOR(Engine);

		// Scene
		void RunScene();
		void ChangeScene(IScene* scene);

		// System
		void Initialize();
		void Render();
		void Destroy();

		// Component
		template <class Component>
		Component& AddComponent(ObjectId id)
		{
			return _componentSystem.Add<Component>(id);
		}
		template <class Component>
		void DeleteComponent(Object Id)
		{
		}
		template <class Component>
		bool HasComponent(Object id) const
		{
			return false;
		}

		GET_ACCESSOR(BufferManager, Rendering::Manager::BufferManager&, _bufferManager);
		GET_ACCESSOR(DirectX, Device::DirectX&, _dx);

	private:
		static NullScene							_nullScene;
		IScene*										_scene;

		std::chrono::system_clock::time_point		_prevTime;
		double										_lag;

		Rendering::Manager::MaterialManager					_materialManager;
		Rendering::Manager::BufferManager					_bufferManager;
		Rendering::Manager::MeshManager						_meshManager;
		Rendering::Manager::ShaderManager					_shaderManager;
		Rendering::Manager::Texture2DManager				_tex2dManager;
		Core::ObjectManager									_objectManager;
		Device::DirectX&									_dx;
		Core::ComponentSystem<Rendering::Geometry::Mesh>	_componentSystem;
	};
}