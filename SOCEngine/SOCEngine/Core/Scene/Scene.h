#pragma once

#include "Structure.h"
#include "Object.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshImporter.h"
#include "BufferManager.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "BackBufferMaker.h"

namespace Core
{
	class Scene
	{
	public:
		enum class State{ Init = 0, Loop, End, Num };

	private:
		State _state;
		Rendering::PostProcessing::BackBufferMaker*	_backBufferMaker;

	protected:
		Structure::Vector<std::string, Core::Object>	_rootObjects;	

		Rendering::Manager::CameraManager*		_cameraMgr;
		Rendering::Manager::ShaderManager*		_shaderMgr;
		Rendering::Manager::TextureManager*		_textureMgr;
		Rendering::Manager::MaterialManager*	_materialMgr;
		Importer::MeshImporter*					_meshImporter;
		Rendering::Manager::BufferManager*		_bufferManager;
		Core::ObjectManager*					_originObjMgr;
		Rendering::Manager::RenderManager*		_renderMgr;	
		UI::Manager::UIManager*					_uiManager;

		Rendering::Camera::UICamera*			_uiCamera;
		const Device::DirectX*					_dx;

	public:
		Scene(void);
		~Scene(void);

	public:
		void Initialize();
		void Update(float dt);
		void RenderPreview();
		void Render();
		void Destroy();
		void Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard);

	protected:
		virtual void OnInitialize() = 0;
		virtual void OnRenderPreview() = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard) = 0;		
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
		GET_ACCESSOR(BufferManager, Rendering::Manager::BufferManager*, _bufferManager);
		GET_ACCESSOR(OriginObjectManager, Core::ObjectManager*, _originObjMgr);
		GET_ACCESSOR(RenderManager, Rendering::Manager::RenderManager*, _renderMgr);
		GET_ACCESSOR(UIManager, UI::Manager::UIManager*, _uiManager);
	};
}