#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"

using namespace Rendering;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	//Object - Cube
	Factory::ShaderFactory factory(_shaderMgr);
	Core::Object* cube = _meshImporter->Load("./Resource/sphere.obj", "./Resource/");
	cube->GetTransform()->UpdatePosition(Math::Vector3(0, 0, 3));
	_test = cube->GetTransform();
	cube->GetTransform()->UpdateScale(Math::Vector3(1.5, 1.5, 1.5));
	AddObject(cube);

	//Camera
	{
		Core::Object* camObj = new Core::Object(nullptr);
		camObj->SetName("MainCam");
		Camera::Camera* cam = camObj->AddComponent<Camera::Camera>();
		Core::Transform* camObjTf = camObj->GetTransform();
		camObjTf->UpdatePosition(Math::Vector3(0,0,0));
		camObjTf->UpdateDirection(Math::Vector3(0,0,1));
		_cameraMgr->Add("mainCamera", cam, false);
	}
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnUpdate(float dt)
{
	static float t = 3.0f;
	t+=0.001f;
	_test->UpdatePosition(Math::Vector3(0,0,t));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}