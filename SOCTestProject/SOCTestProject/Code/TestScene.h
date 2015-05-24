#pragma once

#include "Scene.h"
#include "ShaderFactory.hpp"

class TestScene : public Core::Scene
{
private:
	UI::SimpleImage2D* img;
public:
	TestScene(void);
	~TestScene(void);

public:
	virtual void OnInitialize();
	virtual void OnRenderPreview();
	virtual void OnUpdate(float dt);
	virtual void OnRenderPost();
	virtual void OnDestroy();
};

