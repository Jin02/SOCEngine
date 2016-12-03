#include "CameraManager.h"
#include "Utility.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Core;
using namespace Rendering::Manager;
using namespace Rendering::Camera;
using namespace Structure;

CameraManager::CameraManager(){}

CameraManager::~CameraManager(){}

void CameraManager::Add(Camera::CameraForm* camera)
{
	address key = reinterpret_cast<address>(camera);
	ASSERT_COND_MSG(Has(key) == false, "Error, Already registed camera objeect");

	VectorMap<address, Camera::CameraForm*>::Add(key, camera);
}

void CameraManager::Delete(Camera::CameraForm* camera)
{
	address key = reinterpret_cast<address>(camera);
	VectorMap<address, Camera::CameraForm*>::Delete(key);
	SAFE_DELETE(camera);
}

void CameraManager::DeleteAll()
{
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
	{
		//당연한거긴 하다만.. 에러난다.
		//메모리 참조, 관리 등등 이슈관리할때 처리하자.

		ASSERT_MSG("Bug");

		CameraForm* cam = (*iter);
		SAFE_DELETE(cam);
	}

	VectorMap<address, Camera::CameraForm*>::DeleteAll();
}

void CameraManager::Destroy()
{
	DeleteAll();
}

void CameraManager::SetMainCamera(Camera::CameraForm* cam)
{
	CameraForm* curMainCam = GetMainCamera();
	if(curMainCam == nullptr)
	{
		Add(cam);
		return;
	}

	address key = reinterpret_cast<address>(cam);
	uint vecSwapIdx = -1;
	Find(key, &vecSwapIdx);

	// Swap Vector Element
	{
		if(vecSwapIdx != -1)
			std::iter_swap(_vector.begin(), _vector.begin() + vecSwapIdx);
		else
		{
			Add(cam);
			std::iter_swap(_vector.begin(), _vector.end() - 1);
		}
	}

	// Swap Map Element
	{
		uint mainCamKey		= reinterpret_cast<address>(GetMainCamera());

		auto mainCamIter	= _map.find(mainCamKey);
		auto swapCamIter	= _map.find(key);

		uint t = mainCamIter->second;
		mainCamIter->second = swapCamIter->second;
		swapCamIter->second = t;
	}
}

Camera::CameraForm* CameraManager::GetMainCamera() const
{
	return (GetSize() != 0) ? (*_vector.begin()) : nullptr;
}