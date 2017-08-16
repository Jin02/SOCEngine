#include "CameraManager.h"
#include "Object.h"

using namespace Rendering::Camera;
using namespace Rendering::Manager;

CameraManager::CameraManager()
	: _mainCamera(Core::ObjectID())
{

}

void CameraManager::Initialize(Device::DirectX & dx, ShaderManager & shaderMgr, const Rect<uint>& mainCamRenderRect)
{
	_mainCamera.Initialize(dx, shaderMgr, mainCamRenderRect);
}

void Rendering::Manager::CameraManager::SetMainCamera(Core::ObjectID objectID)
{
	_mainCamera.SetObjectID(objectID);
}

void CameraManager::DeleteAll()
{
	// Nothing
}
