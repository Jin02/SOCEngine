#pragma once

#include "Scene.h"
#include "ShaderFactory.hpp"
#include "SimpleSoundPlayer.h"
#include "SimpleText2D.h"

class TestScene : public Core::Scene
{
private:
	UI::SimpleImage2D* img;
	std::auto_ptr<Sound::SimpleSoundPlayer> sound;
	UI::SimpleText2D* text;

public:
	TestScene(void);
	~TestScene(void);

public:
	virtual void OnInitialize();
	virtual void OnRenderPreview();
	virtual void OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard);
	virtual void OnUpdate(float dt);
	virtual void OnRenderPost();
	virtual void OnDestroy();
};

