#include "CameraManager.h"
#include "Object.h"

using namespace Math;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Manager;

CameraManager::CameraManager()
	: _mainCamera(Core::ObjectID())
{

}

bool CameraManager::CheckCanUseMainCam(const ObjectIDManager& objIDMgr) const
{
	return objIDMgr.Has(_mainCamera.GetObjectID());
}

void CameraManager::SetMainCamera(Core::ObjectID objectID)
{
	_mainCamera.SetObjectID(objectID);
}

bool CameraManager::InFrustumAllCamera(const Vector3& worldPos, float radius) const
{
	if (_mainCamera.GetFrustum().In(worldPos, radius))
		return true;

	// TODO : 이제 여기에 다양한 카메라들을 집어넣으면 된다.

	return false;
}

void CameraManager::DeleteAll()
{
	// Nothing
}
