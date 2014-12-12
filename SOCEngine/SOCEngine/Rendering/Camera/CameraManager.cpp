#include "CameraManager.h"

using namespace Rendering;
using namespace Core;
using namespace Rendering::Manager;

CameraManager::CameraManager()
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::SetMainCamera(Camera::Camera *cam)
{
	_vector.insert(_vector.begin(), Vector::Type("Main", Vector::Data(false, cam)));
}

Camera::Camera* CameraManager::GetMainCamera()
{
	return _vector.size() != 0 ? (*_vector.begin()).second.second : nullptr;
}

void CameraManager::Render(std::vector<Object*>::iterator& objectBegin,
			std::vector<Object*>::iterator& objectEnd, Manager::LightManager* sceneLights)
{

}