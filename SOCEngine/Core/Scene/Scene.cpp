#include "Scene.h"

using namespace Rendering;
using namespace Texture;
using namespace Shader;
using namespace Light;
using namespace std;
using namespace Material;
using namespace Device;

//static Scene* nowScene = nullptr;

Scene::Scene(void) : BaseScene()
{
	destroyMgr = true;
}

Scene::~Scene(void)
{
	Destroy();
}

void Scene::Initialize()
{
	lightMgr		= new LightManager;
	textureMgr		= new TextureManager;
	graphics		= Device::DeviceDirector::GetInstance()->GetGraphics();
	shaderMgr		= new ShaderManager(graphics);
	cameraMgr		= new CameraManager;
	rootObjects		= new Container<Object>;
	meshDataMgr		= new MeshDataManager;
	materialMgr		= new MaterialManager;

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);
	vector<Object*>::iterator end = rootObjects->GetEndIter();

	for(vector<Object*>::iterator iter = rootObjects->GetBeginIter(); iter != end; ++iter)
		(*iter)->Update(dt);
}

void Scene::Render()
{
	Camera *mainCam = cameraMgr->GetMainCamera();
	SOC_dword bgColour = 0xFF0000FF;

	if(mainCam == nullptr)
		return;

	graphics->BeginScene();
	{
		OnRenderPreview();

		graphics->Clear(0, NULL,
			Graphics::ClearFlagType::FlagTarget | 
			Graphics::ClearFlagType::FlagZBuffer, 
			bgColour,
			1.0f, 0);

		//for(vector<Camera*>::iterator iter = cameraMgr->GetIteratorBegin(); iter != cameraMgr->GetIteratorEnd(); ++iter)
		//	(*iter)->Run(dt);
		
		//Camera::SceneRender(mainCam, rootObjects->GetBeginIter(), rootObjects->GetEndIter(), lightMgr);
		//일단은, 이렇게 처리하고 추후에 각 카메라마다 Render to texture 세팅해준뒤, 그걸 처리하도록 해야할듯.
		//추후 조정이 필요함.

		cameraMgr->Render(rootObjects->GetBeginIter(), rootObjects->GetEndIter(), lightMgr);


		OnRenderPost();
	}
	graphics->EndScene();
	graphics->Present();
}

void Scene::Destroy()
{
	if(destroyMgr == false)
		return;

	lightMgr->DeleteAll(true);
	shaderMgr->DeleteAll();
	textureMgr->DeleteAll();
	meshDataMgr->DeleteAll();
	materialMgr->DeleteAll();

	Utility::SAFE_DELETE(lightMgr);
	Utility::SAFE_DELETE(shaderMgr);
	Utility::SAFE_DELETE(textureMgr);
	Utility::SAFE_DELETE(cameraMgr);
	Utility::SAFE_DELETE(meshDataMgr);
	Utility::SAFE_DELETE(materialMgr);

	OnDestroy();
}

LightManager* Scene::GetLightManager()
{
	return lightMgr;
}

TextureManager* Scene::GetTextureManager()
{
	return textureMgr;
}

ShaderManager* Scene::GetShaderManager()
{
	return shaderMgr;
}

CameraManager* Scene::GetCameraManager()
{
	return cameraMgr;
}

MeshDataManager* Scene::GetMeshDataMgr()
{
	return meshDataMgr;
}

MaterialManager* Scene::GetMaterialMgr()
{
	return materialMgr;
}