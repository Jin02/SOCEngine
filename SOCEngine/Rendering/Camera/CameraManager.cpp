#include "CameraManager.h"

using namespace Rendering;
using namespace Core;

CameraManager::CameraManager() : Container()
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::SetMainCamera(Camera *cam)
{
	_objects.insert(_objects.begin(), cam);
}

Camera* CameraManager::GetMainCamera()
{
	return (*_objects.begin());
}

void CameraManager::Render(std::vector<Object*>::iterator& objectBegin,
			std::vector<Object*>::iterator& objectEnd, Light::LightManager* sceneLights)
{
	for(std::vector<Camera*>::iterator iter = _objects.begin();
		iter != _objects.end(); ++iter)
	{
		(*iter)->Render(objectBegin, objectEnd, sceneLights);
	}
}