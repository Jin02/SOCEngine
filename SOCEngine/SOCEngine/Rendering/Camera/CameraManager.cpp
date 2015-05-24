#include "CameraManager.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Core;
using namespace Rendering::Manager;

CameraManager::CameraManager() : _lightCullingCS(nullptr)
{

}

CameraManager::~CameraManager()
{
	SAFE_DELETE(_lightCullingCS);
}

void CameraManager::InitLightCulling()
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