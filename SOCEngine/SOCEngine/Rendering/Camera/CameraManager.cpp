#include "CameraManager.h"
#include "Object.h"

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

void Rendering::Manager::CameraManager::SetMainCamera(Core::ObjectId objectId)
{
	_mainCamera.SetObjectId(objectId);
}

void CameraManager::DeleteAll()
{
	// Nothing
}
