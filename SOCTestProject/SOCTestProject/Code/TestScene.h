#pragma once

#include <IScene.h>
#include <Engine.h>

class TestScene : public Core::IScene
{
public:
	virtual RenderSetting RegistRenderSetting(Core::Engine& engine) override;

	virtual void OnInitialize(Core::Engine&)	override;
	virtual void OnDestroy(Core::Engine&)		override;
	virtual void OnRenderPreview(Core::Engine&)	override;
	virtual void OnUpdate(Core::Engine&)		override;
	virtual void OnRenderPost(Core::Engine&)	override;
	virtual void OnInput(Core::Engine&)			override;
};

