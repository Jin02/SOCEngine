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
	camera = new Object("Default");
	MainCamera* cam = camera->AddComponent<MainCamera>();
	camera->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));
	camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));

	Importer::MeshImporter importer;
	testObject = importer.Load("./Resources/Sphere/sphere.obj");
//	testObject = importer.Load("./Resources/tt.obj");

	//testObject = importer.Load("./Resources/Cube/Cube.obj");
	//Mesh* mesh = testObject->GetChild(0)->GetChild(0)->GetComponent<Mesh>();
	//PhysicallyBasedMaterial* material = (PhysicallyBasedMaterial*)mesh->GetMeshRenderer()->GetMaterials().front();
	//material->UpdateMainColor(Color(1.0f, 1.0f, 1.0f, 0.9f));

	testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
//	testObject->GetTransform()->UpdatePosition(Vector3(0, -0.6f, 1.0f));

	//testObject->GetTransform()->UpdateEulerAngles(Vector3(0, 90, 0));
	//testObject->GetTransform()->UpdateEulerAngles(Vector3(45, 35, 20));
	AddObject(testObject);

	light = new Object("Light");

	//light->AddComponent<PointLight>();
	//light->SetRadius(10.0f);
	//light->GetTransform()->UpdatePosition(Vector3(4, 4, 0));

	SpotLight* sl = light->AddComponent<SpotLight>();
	light->SetRadius(10.0f);
	light->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	sl->SetSpotAngleDegree(5.0f);

	light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));

	AddObject(light);
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
	static float x = 0.0f;

	x += 0.1f;
	testObject->GetTransform()->UpdateEulerAngles(Math::Vector3(45, x, 20));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}