#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"

using namespace Rendering;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	Core::Object* camObj = new Core::Object;
	Camera::Camera* cam = camObj->AddComponent<Camera::ForwardPlusCamera>();
	Device::Director::GetInstance()->GetCurrentScene()->GetCameraManager()->Add("mainCam", cam);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
}

void TestScene::OnUpdate(float dt)
{
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}