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
	Shader::Factory factory(_shaderMgr);

	Shader::VertexShader* vs = nullptr;
	Shader::PixelShader* ps = nullptr;

	if(factory.LoadShader("Test", "VS", "PS", &vs, &ps) == false)
		ASSERT("테스트. 쉐이더가 로딩되지 않음.");

	Shader::ShaderManager* mgr = _shaderMgr;

	_meshImporter->Load("./Resource/cube.obj", "./Resource/");
//	_meshImporter->Load("./Resource/sponza/sponza.obj", "./Resource/sponza/", vs);

	//Camera
	{
		Core::Object* camObj = new Core::Object(nullptr);
		Rendering::Camera* cam = camObj->AddComponent<Rendering::Camera>();
		Core::Transform* camObjTf = camObj->GetTransform();
		camObjTf->UpdatePosition(Math::Vector3(0,0,0));
		camObjTf->UpdateDirection(Math::Vector3(0,0,1));
		_cameraMgr->Add("mainCamera", cam, false);
	}

	{
		Core::Object* testObj = new Core::Object(nullptr);
		Rendering::Mesh::Mesh* mesh = testObj->AddComponent<Rendering::Mesh::Mesh>();
		Core::Transform* tf = testObj->GetTransform();
		_test = tf;
		tf->UpdatePosition(Math::Vector3(0, 0, 10));
		_rootObjects.Add("Test", testObj);
		{

		}
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