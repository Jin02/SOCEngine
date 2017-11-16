#pragma once

#include "RenderSetting.h"
#include <string>

namespace Core
{
	class Engine;

	class IScene
	{
	public:
		virtual void OnRenderPreview()								= 0;
		virtual void OnUpdate()										= 0;
		virtual void OnRenderPost()									= 0;
		virtual void OnInput()										= 0;

		virtual void OnInitialize(Engine& engine)					= 0;
		virtual void OnDestroy(Engine& engine)						= 0;
		virtual RenderSetting RegistRenderSetting(Engine& engine)	= 0;
	};

	class NullScene : public IScene
	{
	public:
		virtual void OnRenderPreview()								override {/* NULL */}
		virtual void OnUpdate()										override {/* NULL */}
		virtual void OnRenderPost()									override {/* NULL */}
		virtual void OnInput()										override {/* NULL */}

		virtual void OnInitialize(Engine& engine)					override {/* NULL */}
		virtual void OnDestroy(Engine& engine)						override {/* NULL */}
		virtual RenderSetting RegistRenderSetting(Engine& engine)	override { return RenderSetting(); }
	};
}