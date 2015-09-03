#include "CameraManager.h"
#include "Utility.h"

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

void CameraManager::SetFirstCamera(Camera::CameraForm *cam)
{
	auto mainCamIter = _vector.begin();
	auto findIter = _vector.begin();

	for(findIter; findIter != _vector.end(); ++findIter)
	{
		if( (*findIter) == cam )
			break;
	}

	ASSERT_COND_MSG(findIter != _vector.end(), "CameraManager does not have a input cam"); 
	std::swap(findIter, mainCamIter);
}

Camera::CameraForm* CameraManager::GetFirstCamera() const
{
	return _vector.empty() ? nullptr : _vector.front();
}