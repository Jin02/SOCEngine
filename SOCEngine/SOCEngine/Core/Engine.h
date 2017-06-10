#pragma once

#include "Common.h"
#include "IScene.h"
#include <memory>
#include <chrono>
#include "MainSystemHeader.h"
#include "ObjectManager.h"
#include "MaterialManager.hpp"
#include "BufferManager.hpp"
#include "ShaderManager.h"
#include "Texture2DManager.h"

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

		GET_ACCESSOR(BufferManager, Rendering::Manager::BufferManager&, _bufferManager);
		GET_ACCESSOR(DirectX, Device::DirectX&, _dx);
		GET_ACCESSOR(ComponentSystem, DefaultComponentSystem&, _componentSystem);
		GET_ACCESSOR(TransformPool, auto&, _transformPool);
		GET_CONST_ACCESSOR(TransformPool, const auto&, _transformPool);

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
		DefaultComponentSystem								_componentSystem;
		Core::TransformPool									_transformPool;

	private:
		std::vector<Core::Transform*>						_dirtyTransforms;
	};
}