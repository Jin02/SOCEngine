#pragma once

#include "Common.h"

namespace Core
{
	class IScene
	{
	public:
		DISALLOW_ASSIGN(IScene);
		DISALLOW_COPY_CONSTRUCTOR(IScene);

	public:
		virtual void OnRenderPreview()		= 0;
		virtual void OnUpdate()				= 0;
		virtual void OnRenderPost()			= 0;
		virtual void OnInput()				= 0;
	};

	class NullScene : public IScene
	{
	public:
		virtual void OnRenderPreview()	override {/* NULL */ }
		virtual void OnUpdate()			override {/* NULL */ }
		virtual void OnRenderPost()		override {/* NULL */ }
		virtual void OnInput()			override {/* NULL */ }
	};
}