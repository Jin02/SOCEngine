#include "Scene.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;

Scene::Scene(void)
{
	_state = State::Init;
}

Scene::~Scene(void)
{
	Destroy();
}

void Scene::Initialize()
{
	_cameraMgr = new CameraManager;

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		GET_CONTENT_FROM_ITERATOR(iter)->Update(dt);
}

void Scene::Render()
{
	Camera *mainCam = _cameraMgr->GetMainCamera();

	if(mainCam == nullptr)
		return;

	//graphics->BeginScene();
	{
		OnRenderPreview();

		/*graphics->Clear(0, NULL,
			Graphics::ClearFlagType::FlagTarget | 
			Graphics::ClearFlagType::FlagZBuffer, 
			bgColour,
			1.0f, 0);*/

		//for(vector<Camera*>::iterator iter = cameraMgr->GetIteratorBegin(); iter != cameraMgr->GetIteratorEnd(); ++iter)
		//	(*iter)->Run(dt);
		
		//Camera::SceneRender(mainCam, rootObjects->GetBeginIter(), rootObjects->GetEndIter(), lightMgr);
		//일단은, 이렇게 처리하고 추후에 각 카메라마다 Render to texture 세팅해준뒤, 그걸 처리하도록 해야할듯.
		//추후 조정이 필요함.

		//cameraMgr->Render(rootObjects->GetBeginIter(), rootObjects->GetEndIter(), lightMgr);

		OnRenderPost();
	}
	//graphics->EndScene();
	//graphics->Present();
}

void Scene::Destroy()
{
	SAFE_DELETE(_cameraMgr);

	OnDestroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}