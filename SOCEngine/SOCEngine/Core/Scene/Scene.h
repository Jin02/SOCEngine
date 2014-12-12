#pragma once

#include "Structure.h"
#include "Object.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "Sampler.h"
#include "MeshImporter.h"
#include "BufferManager.h"

namespace Core
{
	class Scene
	{
	public:
		enum State{ Init = 0, Loop, End, Num };

	private:
		State _state;

	protected:
		Structure::Vector<Core::Object>			_rootObjects;	
		Rendering::Manager::CameraManager*		_cameraMgr;
		Rendering::Manager::ShaderManager*		_shaderMgr;
		Rendering::Manager::TextureManager*		_textureMgr;
		Rendering::Manager::MaterialManager*	_materialMgr;
		Rendering::Sampler*						_sampler;
		Importer::MeshImporter*					_meshImporter;
		Rendering::Manager::BufferManager*		_bufferManager;
		Core::ObjectManager*					_originObjMgr;


	public:
		Scene(void);
		~Scene(void);

	public:
		void Initialize();
		void Update(float dt);
		void Render();
		void Destroy();

	protected:
		virtual void OnInitialize() = 0;
		virtual void OnRenderPreview() = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnRenderPost() = 0;
		virtual void OnDestroy() = 0;

	protected:
		Core::Object* AddObject(Core::Object* object, bool clone = true);

	public:
		void NextState();
		GET_ACCESSOR(State, const State, _state);

		GET_ACCESSOR(CameraManager, Rendering::Manager::CameraManager*, _cameraMgr);
		GET_ACCESSOR(TextureManager, Rendering::Manager::TextureManager*, _textureMgr);
		GET_ACCESSOR(ShaderManager, Rendering::Manager::ShaderManager*, _shaderMgr);
		GET_ACCESSOR(MaterialManager, Rendering::Manager::MaterialManager*, _materialMgr);
		GET_ACCESSOR(Sampler, Rendering::Sampler*, _sampler);
		GET_ACCESSOR(BufferManager, Rendering::Manager::BufferManager*, _bufferManager);
		GET_ACCESSOR(OriginObjectManager, Core::ObjectManager*, _originObjMgr);
	};
}