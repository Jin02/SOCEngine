#include "Engine.h"

using namespace Core;
using namespace std::chrono;

Engine::Engine()
	: _scene(&_nullScene), _prevTime(), _lag(0.0)
{

}

Engine::Engine(IScene* scene)
	: _scene(scene), _prevTime(), _lag(0.0)
{
}

void Engine::RunScene()
{
	auto curTime = system_clock::now();
	duration<double> elapsed = curTime - _prevTime;

	_prevTime = curTime;
	_lag += elapsed.count();

	_scene->OnInput();

	const double MS_PER_UPDATE = 8.0;
	while (_lag >= MS_PER_UPDATE)
	{
		_scene->OnUpdate();
		_lag -= MS_PER_UPDATE;
	}

	_scene->OnRenderPreview();
	Render();
	_scene->OnRenderPost();
}

void Engine::ChangeScene(IScene* scene)
{
	_scene = scene ? scene : &_nullScene;
}

void Engine::Initialize()
{
}

void Engine::Destroy()
{
}

void Engine::Render()
{
}