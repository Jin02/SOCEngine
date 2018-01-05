#include "Engine.h"
#include "DirectX.h"
#include "ObjectManager.h"

#include "CoreConnector.h"
#include <algorithm>

#undef min
#undef max

using namespace Core;
using namespace Importer;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Manager;

Engine::Engine(Device::DirectX& dx)
	: _dx(dx), _scene(&_nullScene), _util(*this)
{
}

void Engine::RunScene()
{
	_scene->OnInput(*this, _util);

	clock_t curTime = clock();
	clock_t elapsed = curTime - _prevTime;

	_elapsedTimeForUpdate += elapsed;

	while (_elapsedTimeForUpdate >= _perUpdate)
	{
		_scene->OnUpdate(*this, _util);
		_elapsedTimeForUpdate -= _perUpdate;
	}

	UpdateWorldMatrix();

	// check dirty transform
	{
		_transformPool.Iterate(
			[&dirtys = _dirtyTransforms](Transform& tf)
			{
				if (tf.GetDirty())
					dirtys.push_back(&tf);
			}
		);
	}

	_componentSystem.UpdateBuffer(_dx, _transformPool, _objectManager, _rendering.GetShadowAtlasMapRenderer());

	_scene->OnRenderPreview(*this, _util);

	++_frameCount;
	if (_elapsedTimePerFrame >= 1.0f)
	{
		_fps = static_cast<float>(_frameCount) / _elapsedTimePerFrame;

		_elapsedTimePerFrame	= 0.0f;
		_frameCount				= 0;
	}

	float dt = static_cast<float>(clock() - _prevTime) / 1000.0f;
	_elapsedTimePerFrame += dt;
	_rendering.Render(*this, std::max(dt, 0.0f));

	_scene->OnRenderPost(*this, _util);

	_prevTime = curTime;

	for (auto& iter : _dirtyTransforms)
		iter->ClearDirty();

	_dirtyTransforms.clear();
	_componentSystem.ClearDirty();
}

void Engine::UpdateWorldMatrix()
{
	for (ObjectID rootID : _rootObjectIDs.GetVector())
	{
		Transform* find = _transformPool.Find(rootID.Literal());	assert(find);
		find->UpdateWorldMatrix(_transformPool);
	}
}

void Engine::ChangeScene(IScene* scene)
{
	_scene = scene ? scene : &_nullScene;
}

void Engine::Initialize(IScene* scene)
{
	CoreConnector::SharedInstance()->Connect(&_transformPool, &_dontUseTransformPool, &_objectManager, &_componentSystem);

	auto renderSetting = scene->RegistRenderSetting(*this, _util);
	_rendering.InitializeRenderer(*this, std::move(renderSetting));

	_componentSystem.Initialize(_dx);
	_rendering.Initialize(*this, renderSetting.useBloom);

	scene->OnInitialize(*this, _util);

	ChangeScene(scene);
}

void Engine::Destroy()
{
	_scene->OnDestroy(*this, _util);
	_exit = true;
}

void Engine::AddRootObject(const Object& object)
{
	_rootObjectIDs.Add(object.GetObjectID());
}