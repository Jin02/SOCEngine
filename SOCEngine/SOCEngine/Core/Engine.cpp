#include "Engine.h"
#include "DirectX.h"
#include "ObjectManager.h"

#include "CoreConnector.h"

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

	_lag += elapsed;

	constexpr clock_t MS_PER_UPDATE = 100;
	while (_lag >= MS_PER_UPDATE)
	{
		_scene->OnUpdate(*this, _util);
		_lag -= MS_PER_UPDATE;
	}

	UpdateWorldMatrix();

	// check dirty transform
	{
		uint size = _transformPool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& tf = _transformPool.Get(i);

			if (tf.GetDirty())
				_dirtyTransforms.push_back(&tf);
		}
	}

	_componentSystem.UpdateBuffer(_dx, _transformPool, _objectManager, _rendering.GetShadowAtlasMapRenderer());

	_scene->OnRenderPreview(*this, _util);
	_rendering.Render(*this, std::max(float(clock() - _prevTime) / 1000.0f, 0.0f));
	_scene->OnRenderPost(*this, _util);

	for (auto& iter : _dirtyTransforms)
		iter->ClearDirty();

	_dirtyTransforms.clear();
	_componentSystem.ClearDirty();

	_prevTime = curTime;
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