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

#define USE_SPONZA_TEST

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
#if defined(USE_SPONZA_TEST)
	_camera = new Object("Default");
	MeshCamera* cam = _camera->AddComponent<MeshCamera>();
	_camera->GetTransform()->UpdateEulerAngles(Vector3(0, 270, 0));

	Vector3 camPos = Vector3(-1500, 190, -30);
	_camera->GetTransform()->UpdatePosition(camPos);

	Importer::MeshImporter importer;
	_testObject = importer.Load("./Resources/Sponza/sponzafbx.fbx");
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 0, 0));
	AddObject(_testObject);

	auto AddPointLight = [&](float x, float y, float z)
	{
		static int idx = 0;
		Core::Object* light = new Object("Light" + std::to_string(idx++));
		PointLight* pl = light->AddComponent<PointLight>();

		Vector3 dir = light->GetTransform()->GetForward();
		light->GetTransform()->UpdatePosition(Vector3(x, y, z));

		pl->SetLumen(700);
		pl->SetRadius(50.0f);

		AddObject(light);
	};
	auto AddPointLightVec3 = [&](const Vector3& worldPos)
	{
		AddPointLight(worldPos.x, worldPos.y, worldPos.z);
	};

	AddPointLightVec3(camPos);

#else
	_camera = new Object("Default");
	MeshCamera* cam = _camera->AddComponent<MeshCamera>();
	_camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));

	Importer::MeshImporter importer;
#if 0
	_testObject = importer.Load("./Resources/Capsule/capsule.obj");
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
#else
	_testObject = importer.Load("./Resources/House/SanFranciscoHouse.fbx");
	_testObject->GetTransform()->UpdatePosition(Vector3(0, -5, 15));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 0, 0));
#endif
	AddObject(_testObject);

	Core::Object* light = new Object("Light");
	light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));

	Vector3 dir = light->GetTransform()->GetForward();
	light->GetTransform()->UpdatePosition(Vector3(0, 0, 0));

	DirectionalLight* spotLight = light->AddComponent<DirectionalLight>();
	//spotLight->SetLumen(700);
	//spotLight->SetRadius(20.0f);
	//spotLight->SetSpotAngleDegree(25.0f);
	spotLight->SetIntensity(2.0f);
	spotLight->ActiveShadow(true);

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
		Vector3 pos = _camera->GetTransform()->GetLocalPosition();
		_camera->GetTransform()->UpdatePosition(pos + Vector3(0, 10, 0));
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = _camera->GetTransform()->GetLocalPosition();
		_camera->GetTransform()->UpdatePosition(pos + Vector3(-10, 0, 0));
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = _camera->GetTransform()->GetLocalPosition();
		_camera->GetTransform()->UpdatePosition(pos + Vector3(0, -10, 0));
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = _camera->GetTransform()->GetLocalPosition();
		_camera->GetTransform()->UpdatePosition(pos + Vector3(10, 0, 0));
	}

	if(keyboard.states['T'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = _camera->GetTransform()->GetLocalPosition();
		_camera->GetTransform()->UpdatePosition(pos + Vector3(0, 0, 10));
	}
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = _camera->GetTransform()->GetLocalPosition();
		_camera->GetTransform()->UpdatePosition(pos + Vector3(0, 0, -10));
	}
}

void TestScene::OnUpdate(float dt)
{
//#ifndef USE_SPONZA_TEST
//	static float x = 0.0f;
//
//	x += 0.1f;
//	_testObject->GetTransform()->UpdateEulerAngles(Math::Vector3(90, x, 0));
//#endif
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}