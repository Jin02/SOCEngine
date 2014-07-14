#include "CameraManager.h"

using namespace Rendering;

CameraManager::CameraManager() : Container()
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::SetMainCamera(Camera *cam)
{
	objects.insert(objects.begin(), cam);
}

Camera* CameraManager::GetMainCamera()
{
	return (*objects.begin());
}

void CameraManager::Render(std::vector<Object*>::iterator& objectBegin,
			std::vector<Object*>::iterator& objectEnd, Light::LightManager* sceneLights)
{
	for(std::vector<Camera*>::iterator iter = objects.begin();
		iter != objects.end(); ++iter)
	{
		(*iter)->Render(objectBegin, objectEnd, sceneLights);
	}
}