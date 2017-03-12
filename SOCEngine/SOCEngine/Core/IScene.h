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
		virtual void OnRenderPreview()	{/* NULL */}
		virtual void OnUpdate()			{/* NULL */}
		virtual void OnRenderPost()		{/* NULL */}
		virtual void OnInput()			{/* NULL */}
	};
}