#pragma once

#include <IScene.h>
#include <Engine.h>

class TestScene : public Core::IScene
{
public:
	virtual RenderSetting RegistRenderSetting(Core::Engine& engine) override;
	virtual void OnInitialize(Core::Engine&) override;
	virtual void OnDestroy(Core::Engine&) override;
	virtual void OnRenderPreview() override;
	virtual void OnUpdate() override;
	virtual void OnRenderPost() override;
	virtual void OnInput() override;
};

