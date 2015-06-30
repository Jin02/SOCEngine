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

void CameraManager::SetMainCamera(Camera::CameraForm *cam)
{
	auto mainCamIter = _vector.begin();
	auto findIter = _vector.begin(); //임시용, auto 쉽게 쓰려고 begin씀.

	for(findIter; findIter != _vector.end(); ++findIter)
	{
		if( GET_CONTENT_FROM_ITERATOR(findIter) == cam )
			break;
	}

	ASSERT_COND_MSG(findIter != _vector.end(), "CameraManager does not have a input cam"); 
	std::swap(findIter, mainCamIter);
}

Camera::CameraForm* CameraManager::GetMainCamera()
{
	return _vector.size() != 0 ? (*_vector.begin()).second.second : nullptr;
}