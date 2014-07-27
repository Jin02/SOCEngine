#include "CameraManager.h"

using namespace Rendering;
using namespace Core;

CameraManager::CameraManager()
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::SetMainCamera(Camera *cam)
{
	_vector.insert(_vector.begin(), Vector::Type("Main", Vector::Data(false, cam)));
}

Camera* CameraManager::GetMainCamera()
{
	return _vector.size() != 0 ? (*_vector.begin()).second.second : nullptr;
}

void CameraManager::Render(std::vector<Object*>::iterator& objectBegin,
			std::vector<Object*>::iterator& objectEnd, Light::LightManager* sceneLights)
{
	for(auto iter = _vector.begin();iter != _vector.end(); ++iter)
	{
		(*iter).second.second->Render(objectBegin, objectEnd, sceneLights);
	}
}