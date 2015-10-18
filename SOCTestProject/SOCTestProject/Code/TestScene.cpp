#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"
#include "ResourceManager.h"

#include "MeshImporter.h"
#include "PhysicallyBasedMaterial.h"

using namespace Rendering;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Geometry;
using namespace Rendering::Light;
using namespace Rendering::Geometry;
using namespace Resource;
using namespace Device;
using namespace Math;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
#if defined(USE_SPONZA_TEST)
	camera = new Object("Default");
	MeshCamera* cam = camera->AddComponent<MeshCamera>();
	camera->GetTransform()->UpdateEulerAngles(Vector3(0, 270, 0));
	camera->GetTransform()->UpdatePosition(Vector3(-1500, 190, -30));

	Importer::MeshImporter importer;
	//testObject = importer.Load("./Resources/Capsule/capsule.obj");
	//testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
	testObject = importer.Load("./Resources/Sponza/sponzafbx.fbx");
	testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 0, 0));
	AddObject(testObject);

	light = new Object("Light");
	LightForm* lightCompo = light->AddComponent<DirectionalLight>();
	light->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	light->GetTransform()->UpdateEulerAngles(Vector3(90, 240, 0));
	lightCompo->SetIntensity(2.0f);
	AddObject(light);
#else
	camera = new Object("Default");
	MeshCamera* cam = camera->AddComponent<MeshCamera>();
	camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));

	Importer::MeshImporter importer;
#if 0
	testObject = importer.Load("./Resources/Capsule/capsule.obj");
	testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
#else
	testObject = importer.Load("./Resources/House/SanFranciscoHouse.fbx");
	testObject->GetTransform()->UpdatePosition(Vector3(0, -5, 15));
	testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 0, 0));
#endif
	AddObject(testObject);

	light = new Object("Light");
	LightForm* lightCompo = light->AddComponent<DirectionalLight>();
	light->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));
	lightCompo->SetIntensity(2.0f);
	AddObject(light);
#endif
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = camera->GetTransform()->GetLocalPosition();
		camera->GetTransform()->UpdatePosition(pos + Vector3(0, 10, 0));
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = camera->GetTransform()->GetLocalPosition();
		camera->GetTransform()->UpdatePosition(pos + Vector3(-10, 0, 0));
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = camera->GetTransform()->GetLocalPosition();
		camera->GetTransform()->UpdatePosition(pos + Vector3(0, -10, 0));
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = camera->GetTransform()->GetLocalPosition();
		camera->GetTransform()->UpdatePosition(pos + Vector3(10, 0, 0));
	}

	if(keyboard.states['T'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = camera->GetTransform()->GetLocalPosition();
		camera->GetTransform()->UpdatePosition(pos + Vector3(0, 0, 10));
	}
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = camera->GetTransform()->GetLocalPosition();
		camera->GetTransform()->UpdatePosition(pos + Vector3(0, 0, -10));
	}

	if(keyboard.states['U'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = light->GetTransform()->GetLocalEulerAngle();
		light->GetTransform()->UpdateEulerAngles(e + Vector3(0, 10, 0));
	}
	if(keyboard.states['J'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = light->GetTransform()->GetLocalEulerAngle();
		light->GetTransform()->UpdateEulerAngles(e + Vector3(0, -10, 0));
	}
	if(keyboard.states['H'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = light->GetTransform()->GetLocalEulerAngle();
		light->GetTransform()->UpdateEulerAngles(e + Vector3(10, 0, 0));
	}
	if(keyboard.states['K'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = light->GetTransform()->GetLocalEulerAngle();
		light->GetTransform()->UpdateEulerAngles(e + Vector3(-10, 0, 0));
	}
}

void TestScene::OnUpdate(float dt)
{
#ifndef USE_SPONZA_TEST
	static float x = 0.0f;

	x += 0.1f;
	testObject->GetTransform()->UpdateEulerAngles(Math::Vector3(90, x, 0));
#endif
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}