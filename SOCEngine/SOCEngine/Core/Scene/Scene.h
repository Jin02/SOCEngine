#pragma once

#include "Structure.h"
#include "Object.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "BufferManager.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "LightManager.h"

#include "MeshCamera.h"
#include "BackBufferMaker.h"
#include "ShadowRenderer.h"
#include "GlobalIllumination.h"

#include "BasicGeometryGenerator.h"
#include "SkyForm.h"

#include "PostProcessPipeline.h"
#include "ReflectionProbeManager.h"

namespace Core
{
	class Scene
	{
	public:
		enum class State{ Init = 0, Loop, End, Stop, Num };

	private:
		Rendering::Manager::CameraManager*					_cameraMgr;
		Rendering::Manager::ReflectionProbeManager*			_reflectionManager;

		Rendering::Manager::RenderManager*					_renderMgr;	
		UI::Manager::UIManager*								_uiManager;
		Rendering::Manager::LightManager*					_lightManager;
		Rendering::Manager::MaterialManager*				_materialMgr;
		const Device::DirectX*								_dx;
		Rendering::Shadow::ShadowRenderer*					_shadowRenderer;

		Rendering::Sky::SkyForm*							_sky;
		bool												_ableDeallocSky;

		Rendering::Texture::RenderTexture*					_backBuffer;
		const  Rendering::Texture::Texture2D*				_prevIntegrateBRDFMap;

		State												_state;
		Intersection::BoundBox								_boundBox;
		Math::Matrix										_localMat;
		std::function<void()>								_exitFunc;

		Rendering::PostProcessPipeline*						_postProcessingSystem;

	protected:
		Rendering::GI::GlobalIllumination*					_globalIllumination;
		Structure::VectorMap<std::string, Core::Object*>	_rootObjects;

	public:
		Scene(void);
		virtual ~Scene(void);

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
		void AddObject(Core::Object* object);
		Core::Object* FindObject(const std::string& key);
		void DeleteObject(Core::Object* object);
		void DeleteAllObject();

		void ActivateGI(bool activate, uint dimension, float giSize);

		void ActiveSkyBox(const std::string& materialName, const std::string& cubeMapFilePath);
		void ActiveCustomSky(Rendering::Sky::SkyForm* sky);
		void DeactivateSky();

	public:
		void NextState();
		void StopState();

		GET_ACCESSOR(State, State, _state);

		GET_ACCESSOR(CameraManager,		Rendering::Manager::CameraManager*,		_cameraMgr);
		GET_ACCESSOR(RenderManager,		Rendering::Manager::RenderManager*,		_renderMgr);
		GET_ACCESSOR(UIManager,			UI::Manager::UIManager*,				_uiManager);
		GET_ACCESSOR(LightManager,		Rendering::Manager::LightManager*,		_lightManager);
		GET_ACCESSOR(MaterialManager,	Rendering::Manager::MaterialManager*,	_materialMgr);
		GET_ACCESSOR(ShadowManager,		Rendering::Shadow::ShadowRenderer*,		_shadowRenderer);
		GET_ACCESSOR(Sky,				Rendering::Sky::SkyForm*,				_sky);
	};
}