#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"
#include "ResourceManager.h"

using namespace Rendering;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Light;
using namespace Resource;
using namespace Device;
using namespace Importer;
using namespace Math;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	camera = new Object("Default");
	MainCamera* cam = camera->AddComponent<MainCamera>();
	camera->GetTransform()->UpdateDirection(Vector3(0, 0, 1));

	MeshImporter importer;
	importer.Initialize();

	testObject = importer.Load("./Resources/capsule.obj", "./Resources/");
	testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
	AddObject(testObject);

	light = new Object("Light");
	DirectionalLight* pl = light->AddComponent<DirectionalLight>();
	//pl->SetRadius(3.0f);
	//pl->SetSpotAngleDegree(30.0f);
	//pl->SetFalloff(2.0f);
	light->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
//	light->GetTransform()->UpdatePosition(Vector3(2, 0, 3));
	light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));

	AddObject(light);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
}

void TestScene::OnUpdate(float dt)
{
	//static float x = 0.0f;

	//x += 0.01f;
	//testObject->GetTransform()->UpdateEulerAngles(Math::Vector3(0, x, 0));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}