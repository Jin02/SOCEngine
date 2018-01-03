#pragma once

#include "RenderSetting.h"
#include <string>

namespace Core
{
	class Engine;
	class EngineUtility;

	class IScene
	{
	public:
		virtual void OnRenderPreview(Engine&, EngineUtility&)				= 0;
		virtual void OnUpdate(Engine&, EngineUtility&)						= 0;
		virtual void OnRenderPost(Engine&, EngineUtility&)					= 0;
		virtual void OnInput(Engine&, EngineUtility&)						= 0;

		virtual void OnInitialize(Engine&, EngineUtility&)					= 0;
		virtual void OnDestroy(Engine&, EngineUtility&)						= 0;
		virtual RenderSetting RegistRenderSetting(Engine&, EngineUtility&)	= 0;
	};

	class NullScene : public IScene
	{
	public:
		virtual void OnRenderPreview(Engine&, EngineUtility&)						override {/* NULL */}
		virtual void OnUpdate(Engine&, EngineUtility&)								override {/* NULL */}
		virtual void OnRenderPost(Engine&, EngineUtility&)							override {/* NULL */}
		virtual void OnInput(Engine&, EngineUtility&)								override {/* NULL */}

		virtual void OnInitialize(Engine&, EngineUtility&)							override {/* NULL */}
		virtual void OnDestroy(Engine&, EngineUtility&)								override {/* NULL */}
		virtual RenderSetting RegistRenderSetting(Engine&, EngineUtility&)			override { return RenderSetting(); }
	};
}