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
	: _dx(dx), _scene(&_nullScene)
{
	_prevTime = std::numeric_limits<clock_t>::max();
}

void Engine::RunScene()
{
	_scene->OnInput();

	clock_t curTime = clock();
	clock_t elapsed = curTime - _prevTime;

	_lag += elapsed;

	constexpr clock_t MS_PER_UPDATE = 100;
	while (_lag >= MS_PER_UPDATE)
	{
		_scene->OnUpdate();
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

	// TODO : LightShaftMgr 구현하면 바꿔야함
	ObjectID::IndexHashMap nullIndexer;
	_componentSystem.UpdateBuffer(_dx, _transformPool, _objectManager, _rendering.GetShadowAtlasMapRenderer(), nullIndexer);

	_scene->OnRenderPreview();
	_rendering.Render(*this, std::max(float(clock() - _prevTime) / 1000.0f, 0.0f));
	_scene->OnRenderPost();

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

	const auto& renderSetting = scene->RegistRenderSetting(*this);
	_rendering.InitializeRenderer(*this, scene->RegistRenderSetting(*this));

	_componentSystem.Initialize(_dx);
	_rendering.Initialize(*this);

	scene->OnInitialize(*this);

	ChangeScene(scene);
}

void Engine::Destroy()
{
	_scene->OnDestroy(*this);
	_exit = true;
}

Object* Engine::LoadMesh(const std::string& fileDir, bool useDynamicVB, bool useDynamicIB)
{
	ObjectID id = _importer.Load(
		MeshImporter::ManagerParam{
		_rendering.GetBufferManager(),
		_rendering.GetMaterialManager(),
		_objectManager,
		_transformPool,
		_rendering.GetTexture2DManager(),
		_componentSystem,
		_dx
	},
	fileDir, useDynamicVB, useDynamicIB);

	return (id != ObjectID::Undefined()) ? _objectManager.Find(id) : nullptr;
}

void Engine::AddRootObject(const Object& object)
{
	_rootObjectIDs.Add(object.GetObjectID());
}