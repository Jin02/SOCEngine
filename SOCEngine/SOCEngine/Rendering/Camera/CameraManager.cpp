#include "CameraManager.h"
#include "Object.h"

using namespace Rendering::Camera;
using namespace Rendering::Manager;
using namespace Math;

CameraManager::CameraManager()
	: _mainCamera(Core::ObjectID())
{

}

void CameraManager::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr, const Rect<uint>& mainCamRenderRect)
{
	_mainCamera.Initialize(dx, shaderMgr, mainCamRenderRect);
}

void Rendering::Manager::CameraManager::SetMainCamera(Core::ObjectID objectID)
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
