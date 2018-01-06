#pragma once

#include "EngineUtility.h"

namespace Device
{
	class DirectX;
}

namespace Core
{
	class Engine final
	{
	public:
		friend class EngineUtility;

		Engine(Device::DirectX& dx);

		DISALLOW_ASSIGN(Engine);
		DISALLOW_COPY_CONSTRUCTOR(Engine);

		// Scene
		void RunScene();
		void ChangeScene(IScene* scene);

		// System
		void Initialize(IScene* scene);
		void Destroy();

		// Root
		void AddRootObject(const Core::Object& object);
		void UpdateWorldMatrix();

		void StartLoop() { _prevTime = clock(); }

		GET_ALL_ACCESSOR_REF(DirectX,			_dx);
		GET_ALL_ACCESSOR_REF(ComponentSystem,	_componentSystem);
		GET_ALL_ACCESSOR_REF(TransformPool,		_transformPool);
		GET_ALL_ACCESSOR_REF(RootObjectIDs,		_rootObjectIDs);
		GET_ALL_ACCESSOR_REF(RenderingSystem,	_rendering);
		GET_ALL_ACCESSOR_REF(ObjectManager,		_objectManager);
		GET_ALL_ACCESSOR_REF(Importer,			_importer);

		GET_CONST_ACCESSOR(Exit,	bool,	_exit);

		Rendering::Renderer::CullingParam GetCullingParam() const
		{
			return Rendering::Renderer::CullingParam{
				_componentSystem.GetManager_Direct<Rendering::Manager::MeshManager>(),
				_objectManager,
				_transformPool
			};
		}

		SET_ACCESSOR(UpdateTime,			clock_t,	_perUpdate);
		GET_CONST_ACCESSOR(UpdateTime,		clock_t,	_perUpdate);
		GET_CONST_ACCESSOR(FPS,				float,		_fps);

	private:
		NullScene									_nullScene;
		IScene*										_scene;

		clock_t										_prevTime				= 0;
		clock_t										_perUpdate				= 100;
		clock_t										_elapsedTimeForUpdate	= 0;
		uint										_frameCount				= 0;
		float										_fps					= 0.0f;
		float										_elapsedTimePerFrame	= 0.0f;

		Core::RootObjectIDs							_rootObjectIDs;

		Core::ObjectManager							_objectManager;
		Core::ComponentSystem						_componentSystem;

		Core::TransformPool							_transformPool;
		Core::TransformPool							_dontUseTransformPool;

		Rendering::RenderingSystem					_rendering;

		Importer::MeshImporter						_importer;
		Device::DirectX&							_dx;

	private:
		std::vector<Core::Transform*>				_dirtyTransforms;
		bool										_exit = false;

		EngineUtility								_util;
	};
}