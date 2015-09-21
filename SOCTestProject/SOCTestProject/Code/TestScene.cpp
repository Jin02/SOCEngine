#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"
#include "ResourceManager.h"

#include "MeshImporter.h"

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
	MeshImporter importer;
	importer.Load("./Resources/TurretsPack/turret_1.FBX");

//	camera = new Object("Default");
//	MainCamera* cam = camera->AddComponent<MainCamera>();
//	//camera->GetTransform()->UpdateEulerAngles(Vector3(20, 340, 0));
//	//camera->GetTransform()->UpdatePosition(Vector3(150, 200, 100));
//
//	MeshImporter importer;
//	importer.Initialize();
//
////	testObject = importer.Load("./Resources/Sponza/sponza.obj", "./Resources/Sponza/");
//	//testObject = importer.Load("./Resources/Sphere/sphere.obj", "./Resources/Sphere/");
//	testObject = importer.Load("./Resources/Capsule/capsule.obj");
//
//	testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
////	testObject->GetTransform()->UpdateScale(Vector3(0.1f, 0.1f, 0.1f));
//	testObject->GetTransform()->UpdateEulerAngles(Vector3(0, 90, 0));
//	AddObject(testObject);
//
//	light = new Object("Light");
//	light->AddComponent<DirectionalLight>();
//	light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));
//
//	AddObject(light);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(0, 10, 0));
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(-10, 0, 0));
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(0, -10, 0));
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(10, 0, 0));
	}

	if(keyboard.states['Y'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(10, 0, 0));
	}
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(0, -10, 0));
	}
	if(keyboard.states['H'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(-10, 0, 0));
	}
	if(keyboard.states['J'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(0, 10, 0));
	}
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