#pragma once

#include "Scene.h"

class TestScene : public Core::Scene
{
private:
	Core::Transform* _test;

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

