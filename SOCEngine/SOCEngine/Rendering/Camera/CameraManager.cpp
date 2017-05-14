#include "CameraManager.h"
#include <assert.h>

using namespace Rendering;
using namespace Core;
using namespace Rendering::Manager;
using namespace Rendering::Camera;

std::shared_ptr<MeshCamera> CameraManager::GetMainMeshCamera()
{
	auto& meshCams = _cameraSystem.GetPool<MeshCamera>();
	auto find = meshCams.Find(_mainMeshCamId);
	return find ? std::shared_ptr<MeshCamera>(find, [](MeshCamera*){}) : nullptr;
}

void CameraManager::SetMainMeshCamera(MeshCamera& cam)
{
	auto curMainCam = GetMainMeshCamera();
	if (curMainCam == nullptr)
		_cameraSystem.Add<MeshCamera>(cam.GetObjectId(), cam);
	else assert( _cameraSystem.Has<MeshCamera>(cam.GetObjectId()) );

	_mainMeshCamId = cam.GetObjectId();
}
