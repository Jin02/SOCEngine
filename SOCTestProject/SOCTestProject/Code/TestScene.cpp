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
	_camera = new Object("Default");
	MeshCamera* cam = _camera->AddComponent<MeshCamera>();
	_camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	cam->SetFieldOfViewDegree(60.0f);

	Importer::MeshImporter importer;
#if 0
	_testObject = importer.Load("./Resources/Capsule/capsule.obj");
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
#else
	//_testObject = importer.Load("./Resources/House/SanFranciscoHouse.fbx");
	//_testObject->GetTransform()->UpdatePosition(Vector3(0, -5, 15));
	//_testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 0, 0));

	_testObject = importer.Load("./Resources/CornellBox/box.obj");
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 180.0f, 180));
	_testObject->GetTransform()->UpdateScale(Vector3(5, 5, 5));
#endif
	AddObject(_testObject);

	_light = new Object("Light");
	_light->GetTransform()->UpdateEulerAngles(Vector3(-60, -60, 0));
	_light->GetTransform()->UpdatePosition(Vector3(-1, 1.5,  12));

	SpotLight* light = _light->AddComponent<SpotLight>();
//	light->SetIntensity(1);
	light->SetLumen(150);
	light->SetRadius(10.0f);
	light->ActiveShadow(true);
	light->SetSpotAngleDegree(120.0f);

	AddObject(_light);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	Transform* control = _light->GetTransform();
	float value = 1;

	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, 0.1f, 0));
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(-0.1f, 0, 0));
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, -0.1f, 0));
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0.1f, 0, 0));
	}

	if(keyboard.states['T'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, 0, 0.1f));
	}
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, 0, -0.1f));
	}

	if(keyboard.states['U'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(e + Vector3(5, 0, 0));
	}
	if(keyboard.states['J'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(e + Vector3(-5, 0, 0));
	}
	if(keyboard.states['H'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(e + Vector3(0, 5, 0));
	}
	if(keyboard.states['K'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(e + Vector3(0, -5, 0));
	}


}

void TestScene::OnUpdate(float dt)
{
	//Transform* tf = _testObject->GetTransform();

	//Vector3 euler = tf->GetLocalEulerAngle();
	//tf->UpdateEulerAngles(euler - Vector3(0, 0.1f, 0));

	//Vector3 pos = tf->GetLocalPosition();
	//tf->UpdatePosition(pos - Vector3(0, 0.2f, 0));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}