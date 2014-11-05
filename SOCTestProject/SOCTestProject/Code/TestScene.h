#pragma once

#include "Scene.h"
#include "ShaderFactory.hpp"

class TestScene : public Core::Scene
{
private:
	Core::Transform*					_test;
	Rendering::Shader::ShaderFactory*	_factory;

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

