#include "ReflectionProbeManager.h"
#include "Utility.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Core;
using namespace Rendering::Manager;
using namespace Rendering::Camera;
using namespace Structure;

ReflectionProbeManager::ReflectionProbeManager(){}

ReflectionProbeManager::~ReflectionProbeManager(){}

void ReflectionProbeManager::Add(Camera::ReflectionProbe* camera)
{
	address key = reinterpret_cast<address>(camera);
	ASSERT_MSG_IF(Has(key) == false, "Error, Already registed reflection probe objeect");

	VectorMap<address, Camera::ReflectionProbe*>::Add(key, camera);
}

void ReflectionProbeManager::Delete(Camera::ReflectionProbe* camera)
{
	address key = reinterpret_cast<address>(camera);
	VectorMap<address, Camera::ReflectionProbe*>::Delete(key);
	SAFE_DELETE(camera);
}

void ReflectionProbeManager::DeleteAll()
{
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
	{
		ReflectionProbe* cam = (*iter);
		SAFE_DELETE(cam);
	}

	VectorMap<address, Camera::ReflectionProbe*>::DeleteAll();
}

void ReflectionProbeManager::Destroy()
{
	DeleteAll();
}