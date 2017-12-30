#pragma once

#include "RenderSetting.h"
#include <string>

namespace Core
{
	class Engine;

	class IScene
	{
	public:
		virtual void OnRenderPreview(Engine&)				= 0;
		virtual void OnUpdate(Engine&)						= 0;
		virtual void OnRenderPost(Engine&)					= 0;
		virtual void OnInput(Engine&)						= 0;

		virtual void OnInitialize(Engine&)					= 0;
		virtual void OnDestroy(Engine&)						= 0;
		virtual RenderSetting RegistRenderSetting(Engine&)	= 0;
	};

	class NullScene : public IScene
	{
	public:
		virtual void OnRenderPreview(Engine&)						override {/* NULL */}
		virtual void OnUpdate(Engine&)								override {/* NULL */}
		virtual void OnRenderPost(Engine&)							override {/* NULL */}
		virtual void OnInput(Engine&)								override {/* NULL */}

		virtual void OnInitialize(Engine&)							override {/* NULL */}
		virtual void OnDestroy(Engine&)								override {/* NULL */}
		virtual RenderSetting RegistRenderSetting(Engine&)			override { return RenderSetting(); }
	};
}