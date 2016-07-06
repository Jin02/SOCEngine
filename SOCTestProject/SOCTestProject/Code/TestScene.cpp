#include "TestScene.h"
#include <fstream>
#include <ShaderManager.h>
#include <Mesh.h>
#include <LightCulling.h>

#include <Director.h>
#include <ResourceManager.h>

#include <PhysicallyBasedMaterial.h>
#include <SkyBox.h>

using namespace Rendering;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Geometry;
using namespace Rendering::Light;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Sky;
using namespace Resource;
using namespace Device;
using namespace Math;
//using namespace Importer;

TestScene::TestScene(void) : _testObject2(nullptr)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	_camera = new Object("Default");
	MeshCamera* cam = _camera->AddComponent<MeshCamera>();

#if 1 //GI Test
	ActivateGI(true, 256, 50.0f);

	const ResourceManager* resourceMgr	= ResourceManager::SharedInstance();
	Importer::MeshImporter* importer	= resourceMgr->GetMeshImporter();
	_testObject = importer->Load("./Resources/CornellBox/box.obj", false);
	_testObject->GetTransform()->UpdatePosition(Vector3(0.3f, -4.7f, 17.7f));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(-90.0f, 0.0f, 180.0f));
	_testObject->GetTransform()->UpdateScale(Vector3(5.0f, 5.0f, 5.0f));
	AddObject(_testObject);

	_light = new Object("Light");
	_light->GetTransform()->UpdatePosition(Vector3(0.0f, 2.0f, 16.0f));

	PointLight* light = _light->AddComponent<PointLight>();
	light->SetLumen(100);
	light->SetRadius(30.0f);
	light->ActiveShadow(true);
	light->GetShadow()->SetUnderScanSize(0.0f);
	AddObject(_light);

#else //IBL Test
	// SkyBox
	ActiveSkyBox("@Skybox", "Resources/CubeMap/desertcube1024.dds");
	_camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	cam->SetFieldOfViewDegree(60.0f);	

	BasicGeometryGenerator gen;
	uint flag = uint(RenderManager::DefaultVertexInputTypeFlag::UV0) |
				uint(RenderManager::DefaultVertexInputTypeFlag::NORMAL);
	_testObject = gen.CreateSphere(1.0f, 30, 30, flag);
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0.0f, 11.0f));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(0.0f, 0.0f, 0.0f));
	_testObject->GetTransform()->UpdateScale(Vector3(5, 5, 5));
	AddObject(_testObject);

	_light = new Object("Light");
	_light->GetTransform()->UpdateEulerAngles(Vector3(90.0f, 0.0f, 0));
	_light->GetTransform()->UpdatePosition(_camera->GetTransform()->GetLocalPosition());

	DirectionalLight* light = _light->AddComponent<DirectionalLight>();
	light->SetIntensity(2.0f);
	light->SetUseAutoProjectionLocation(true);
	light->ActiveShadow(false);
	AddObject(_light);
#endif
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	Transform* control = _testObject->GetTransform();
	const float scale = 0.1f;

	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, scale, 0));
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, -scale, 0));
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(-scale, 0, 0));
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(scale, 0, 0));
	}

	if(keyboard.states['T'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, 0, scale));
	}
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = control->GetLocalPosition();
		control->UpdatePosition(pos + Vector3(0, 0, -scale));
	}

	if(keyboard.states['U'] == Win32::Keyboard::Type::Up)
	{
		//PointLight* pl = _light->GetComponent<PointLight>();
		//float us = pl->GetShadow()->GetUnderScanSize();
		//pl->GetShadow()->SetUnderScanSize(us + scale);
		Vector3 euler = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(euler + Vector3(5.0f, 0.0f, 0.0f));
	}
	if(keyboard.states['J'] == Win32::Keyboard::Type::Up)
	{
		//PointLight* pl = _light->GetComponent<PointLight>();
		//float us = pl->GetShadow()->GetUnderScanSize();
		//pl->GetShadow()->SetUnderScanSize(us - scale);
		Vector3 euler = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(euler - Vector3(5.0f, 0.0f, 0.0f));
	}
	if(keyboard.states['H'] == Win32::Keyboard::Type::Up)
	{
		//PointLight* pl = _light->GetComponent<PointLight>();
		//pl->GetShadow()->SetBias(pl->GetShadow()->GetBias() - 0.01f);
		Vector3 euler = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(euler - Vector3(0.0f, 5.0f, 0.0f));
	}
	if(keyboard.states['K'] == Win32::Keyboard::Type::Up)
	{
		//PointLight* pl = _light->GetComponent<PointLight>();
		//pl->GetShadow()->SetBias(pl->GetShadow()->GetBias() + 0.01f);
		Vector3 euler = control->GetLocalEulerAngle();
		control->UpdateEulerAngles(euler + Vector3(0.0f, 5.0f, 0.0f));
	}
	if(keyboard.states['M'] == Win32::Keyboard::Type::Up)
	{
		_testObject->SetUse(!_testObject->GetUse());
	}
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