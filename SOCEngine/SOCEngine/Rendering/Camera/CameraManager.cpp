#include "CameraManager.h"

using namespace Rendering::Camera;
using namespace Rendering::Manager;
using namespace Rendering::Manager;

CameraManager::CameraManager()
	: _mainCamera(Core::ObjectId())
{

}

void CameraManager::Initialize(Device::DirectX & dx, ShaderManager & shaderMgr)
{
	_mainCamera.Initialize(dx, shaderMgr);
}

void Rendering::Manager::CameraManager::SetMainCamera(Core::Object object)
{
	_mainCamera.SetObjectId(object.GetId());
}
