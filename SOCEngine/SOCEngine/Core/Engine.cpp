#include "Engine.h"
#include "DirectX.h"
#include "ObjectManager.h"

using namespace Core;
using namespace std::chrono;
using namespace Rendering::Manager;
using namespace Rendering;

Engine::Engine(Device::DirectX& dx)
	: _dx(dx), _scene(&_nullScene), _componentSystem(), _objectManager(&_componentSystem)
{
}

Engine::Engine(Device::DirectX& dx, IScene* scene)
	: _dx(dx), _scene(scene), _componentSystem(), _objectManager(&_componentSystem)
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

	// check dirty transform
	{
		auto& tfPool = _componentSystem.GetTransformPool();
		uint size = tfPool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& tf = tfPool.Get(i);
			if( tf.GetDirty() )
				_dirtyTransforms.push_back(&tf);
		}
	}

	_scene->OnRenderPreview();
	Render();
	_scene->OnRenderPost();

	_dirtyTransforms.clear();
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