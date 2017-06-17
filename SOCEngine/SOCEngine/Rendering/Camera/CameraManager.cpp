#include "CameraManager.h"

using namespace Rendering::Camera;
using namespace Rendering::Manager;

CameraManager::CameraManager()
	: _mainCamera(Core::ObjectId())
{

}

void CameraManager::Initialize(Device::DirectX & dx, ShaderManager & shaderMgr, const Rect<uint>& mainCamRenderRect)
{
	_mainCamera.Initialize(dx, shaderMgr, mainCamRenderRect);
}

void Rendering::Manager::CameraManager::SetMainCamera(Core::Object object)
{
	_mainCamera.SetObjectId(object.GetId());
}
