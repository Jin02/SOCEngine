#pragma once

#include "Structure.h"
#include "Object.h"
#include "CameraManager.h"

namespace Core
{
	class Scene
	{
	public:
		enum State{ Init = 0, Loop, End, Num };

	private:
		State _state;

	protected:
		Structure::Vector<Core::Object>		_rootObjects;	
		Rendering::CameraManager*			_cameraMgr;

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

	public:
		void NextState();
		GET_ACCESSOR(State, const State, _state);
	};
}