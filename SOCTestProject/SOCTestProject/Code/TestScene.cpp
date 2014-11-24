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
	Shader::Factory factory(_shaderMgr);
	Core::Object* cube = _meshImporter->Load("./Resource/rabbit.obj", "./Resource/");
	cube->GetTransform()->UpdatePosition(Math::Vector3(0, 0, 1));
	AddObject(cube);

	//Camera
	{
		Core::Object* camObj = new Core::Object(nullptr);
		Rendering::Camera* cam = camObj->AddComponent<Rendering::Camera>();
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
	//static float t = 0.0f;
	//t+=0.01f;
	//_test->UpdatePosition(Math::Vector3(0,t,0));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}