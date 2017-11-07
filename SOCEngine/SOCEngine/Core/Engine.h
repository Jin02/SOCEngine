#pragma once

#include "Common.h"
#include "IScene.h"
#include <memory>
#include <chrono>
#include "ComponentSystem.h"
#include "ObjectManager.h"
#include "MaterialManager.h"
#include "BufferManager.hpp"
#include "ShaderManager.h"
#include "Texture2DManager.h"
#include "PostProcessPipeline.h"
#include "RootObjectIDs.hpp"

#include "Singleton.h"

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

		GET_ACCESSOR(BufferManager,		Rendering::Manager::BufferManager&, _bufferManager);
		GET_ACCESSOR(DirectX,			Device::DirectX&,					_dx);
		GET_ACCESSOR(ComponentSystem,	Core::ComponentSystem&,				_componentSystem);
		GET_ACCESSOR(TransformPool,		Core::TransformPool&,				_transformPool);
		GET_ACCESSOR(RootObjectIDs,		Core::RootObjectIDs&,				_rootObjectIDs);

	private:
		static NullScene							_nullScene;
		IScene*										_scene;

		clock_t										_prevTime	= 0;
		clock_t										_lag		= 0;

		Core::RootObjectIDs							_rootObjectIDs;

		Rendering::Manager::MaterialManager			_materialManager;
		Rendering::Manager::BufferManager			_bufferManager;
		Rendering::Manager::ShaderManager			_shaderManager;
		Rendering::Manager::Texture2DManager		_tex2dManager;
		Core::ObjectManager							_objectManager;
		Device::DirectX&							_dx;
		Core::ComponentSystem						_componentSystem;
		Core::TransformPool							_transformPool;
		Rendering::Manager::PostProcessPipeline		_postProcessing;

	private:
		std::vector<Core::Transform*>				_dirtyTransforms;
	};
}