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
}

void Engine::RunScene()
{
	_scene->OnInput(*this);

	clock_t curTime = clock();
	clock_t elapsed = curTime - _prevTime;

	_lag += elapsed;

	constexpr clock_t MS_PER_UPDATE = 100;
	while (_lag >= MS_PER_UPDATE)
	{
		_scene->OnUpdate(*this);
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

	_scene->OnRenderPreview(*this);
	_rendering.Render(*this, std::max(float(clock() - _prevTime) / 1000.0f, 0.0f));
	_scene->OnRenderPost(*this);

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

MaterialID Engine::ActivateSkyScattering(uint resolution, const Object& directionalLight)
{
	return _rendering.ActivateSkyScattering(*this, resolution, directionalLight);
}

void Engine::DeactivateSkyScattering()
{
	_rendering.DeactivateSkyScattering();
}

void Engine::ChangeScene(IScene* scene)
{
	_scene = scene ? scene : &_nullScene;
}

void Engine::Initialize(IScene* scene)
{
	CoreConnector::SharedInstance()->Connect(&_transformPool, &_dontUseTransformPool, &_objectManager, &_componentSystem);

	auto renderSetting = scene->RegistRenderSetting(*this);
	_rendering.InitializeRenderer(*this, std::move(renderSetting));

	_componentSystem.Initialize(_dx);
	_rendering.Initialize(*this, renderSetting.useBloom);

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