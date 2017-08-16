#include "Engine.h"
#include "DirectX.h"
#include "ObjectManager.h"

using namespace Core;
using namespace Rendering::Manager;
using namespace Rendering;

Engine::Engine(Device::DirectX& dx)
	: _dx(dx), _scene(&_nullScene)
{
}

Engine::Engine(Device::DirectX& dx, IScene* scene)
	: _dx(dx), _scene(scene)
{
} 

void Engine::RunScene()
{
	clock_t curTime = clock();
	clock_t elapsed = curTime - _prevTime;

	_prevTime = curTime;
	_lag += elapsed;

	_scene->OnInput();

	constexpr clock_t MS_PER_UPDATE = 100;
	while (_lag >= MS_PER_UPDATE)
	{
		_scene->OnUpdate();
		_lag -= MS_PER_UPDATE;
	}

	// check dirty transform
	{
		uint size = _transformPool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& tf = _transformPool.Get(i);
			tf.Update(_transformPool);

			if (tf.GetDirty())
				_dirtyTransforms.push_back(&tf);

			if (tf.GetParentState() == Transform::ParentState::NowRoot)
				_objectManager.AddNewRootObject(tf.GetObjectId());
			else if (tf.GetParentState() == Transform::ParentState::NowChild)
				_objectManager.DeleteRootObject(tf.GetObjectId());
		}
	}

	// Update Root Transform
	{
		_objectManager.CheckRootObjectIDs(_transformPool);
		auto& rootIds = _objectManager.GetRootObjectIDs();
		for (uint rootId : rootIds)
		{
			auto transform = _transformPool.Find(rootId); assert(transform);
			transform->Update(_transformPool);
		}
	}

	// TODO : LightShaftMgr �����ϸ� �ٲ����
	ObjectId::IndexHashMap nullIndexer;
	_componentSystem.UpdateBuffer(_dx, _transformPool, nullIndexer);

	_scene->OnRenderPreview();
	Render();
	_scene->OnRenderPost();

	for (auto& iter : _dirtyTransforms)
		iter->ClearDirty();

	_dirtyTransforms.clear();

	_materialManager.UpdateConstBuffer(_dx);

	float dt = static_cast<float>(clock() - _prevTime) / static_cast<float>(CLOCKS_PER_SEC);
	_postProcessing.SetElapsedTime(dt);
	_postProcessing.UpdateCB(_dx);
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