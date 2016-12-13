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
	PostProcessing::DepthOfField::ParamCB param;
	param.start	= 15.0f;
	param.end	= 20.0f;
//	param.start	= 40.0f;
//	param.end	= 40.0f;

	GetPostProcessing()->GetDepthOfField()->UpdateParam(param);

	_camera = new Object("Default");
	MeshCamera* cam = _camera->AddComponent<MeshCamera>();
//	AddObject(_camera);

#if 1 //GI Test
//	ActivateGI(true, 256, 50.0f);
//	ActivateGI(true, 256, 15.0f);

	const ResourceManager* resourceMgr	= ResourceManager::SharedInstance();
	Importer::MeshImporter* importer	= resourceMgr->GetMeshImporter();

	//_testObject2 = importer->Load("./Resources/Sphere/sphere.obj", false);
	//_pos = Vector3(-2.7f, 3.3f, 17.7f);
	//_testObject2->GetTransform()->UpdatePosition(_pos);
	//_testObject2->GetTransform()->UpdateScale(Vector3(2.5f, 2.5f, 2.5f));
	//
	//AddObject(_testObject2);

	_testObject = importer->Load("./Resources/CornellBox/box.obj", false);
	_testObject->GetTransform()->UpdatePosition(Vector3(0.3f, -4.7f, 17.7f));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(-90.0f, 0.0f, 180.0f));
	_testObject->GetTransform()->UpdateScale(Vector3(5.0f, 5.0f, 5.0f));

	AddObject(_testObject);

	_light = new Object("Light");

	PointLight* light = _light->AddComponent<PointLight>();
	light->SetLumen(100);
	light->SetRadius(30.0f);
	light->ActiveShadow(true);
	light->GetShadow()->SetUnderScanSize(0.0f);
	_light->GetTransform()->UpdatePosition(Vector3(0.0f, 2.0f, 16.0f));

	AddObject(_light);

	//DirectionalLight* dl = _light->AddComponent<DirectionalLight>();
	//dl->SetIntensity(1);
	//_light->GetTransform()->UpdateEulerAngles(Vector3(45.0f, 20.0f, 16.0f));
	//_light->GetTransform()->UpdatePosition(Vector3(-10.0f, 7.0f, 16.0f));

//	dl->ActiveShadow(true);
//	dl->SetUseAutoProjectionLocation(true);

	if(_vxgi)
		_vxgi->SetStartCenterWorldPos(_testObject->GetTransform()->GetLocalPosition() + Vector3(0, 5.0f, 0.0f));
#elif 0
	const ResourceManager* resourceMgr	= ResourceManager::SharedInstance();
	Importer::MeshImporter* importer	= resourceMgr->GetMeshImporter();

	//_testObject = importer->Load("./Resources/Sponza/sponza.obj", false);
	//_testObject->GetTransform()->UpdatePosition(Vector3(-15, -20, 70));
	//_testObject->GetTransform()->UpdateEulerAngles(Vector3(270, 90.0f, 0.0f));
	//_testObject->GetTransform()->UpdateScale(Vector3(0.1f, 0.1f, 0.1f));
	_testObject = importer->Load("./Resources/House/SanFranciscoHouse.fbx", false);
	_testObject->GetTransform()->UpdatePosition(Vector3(0, -5, 15));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(-90, -90, 0));

	AddObject(_testObject);
	UpdateBoundBox();

	_camera->GetTransform()->UpdatePosition(GetBoundBox().GetCenter());

	_light = new Object("Light");
	_light->GetTransform()->UpdatePosition(Vector3(0.0f, 2.0f, 16.0f));
	_light->GetTransform()->UpdateEulerAngles(Vector3(30.0f, 330.0f, 0.0f));
	AddObject(_light);

	DirectionalLight* dl = _light->AddComponent<DirectionalLight>();
	dl->SetIntensity(20.0f);
	dl->ActiveShadow(true);

#elif 0 //IBL Test
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
	if(_vxgi)
	{
		auto voxelViwer = _vxgi->GetDebugVoxelViewer();
		
		if(voxelViwer)
		{
			Object* debugVoxels = voxelViwer->GetVoxelsParent();
			if(debugVoxels)
			{
				Object* exist = FindObject(debugVoxels->GetName());
				if( exist == nullptr )
				{
					_testObject->SetUse(false);

					AddObject(debugVoxels);
					_testObject2 = debugVoxels;
//#ifndef USE_ANISOTROPIC_VOXELIZATION
					debugVoxels->GetTransform()->UpdatePosition(Vector3(0.0f, -0.3f, -4.1f));
//#else
//					debugVoxels->GetTransform()->UpdatePosition(Vector3(0.0f, 1.8f, -3.1f));
//#endif
				}
			}
		}
	}
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	const float scale = 5.0f;
#if 0
	Transform* control = _light->GetTransform();

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
#elif 0
	Vector3 pos = _vxgi->GetStartCenterWorldPos();

	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
		_vxgi->SetStartCenterWorldPos(pos + Vector3(0, scale, 0));
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
		_vxgi->SetStartCenterWorldPos(pos + Vector3(0, -scale, 0));
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
		_vxgi->SetStartCenterWorldPos(pos + Vector3(-scale, 0, 0));
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
		_vxgi->SetStartCenterWorldPos(pos + Vector3(scale, 0, 0));

	if(keyboard.states['T'] == Win32::Keyboard::Type::Up)
		_vxgi->SetStartCenterWorldPos(pos + Vector3(0, 0, scale));
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
		_vxgi->SetStartCenterWorldPos(pos + Vector3(0, 0, -scale));
#elif 1

	auto dof = GetPostProcessing()->GetDepthOfField();
	auto param = dof->GetParam();

	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
	{
		param.start += scale;

		dof->UpdateParam(param);
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		param.start -= scale;

		dof->UpdateParam(param);
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		param.end += scale;

		dof->UpdateParam(param);
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		param.end -= scale;

		dof->UpdateParam(param);
	}

#endif
}

void TestScene::OnUpdate(float dt)
{
	//Transform* control = _testObject2->GetTransform();
	//float scale = 20.0f;
	//float distScale = 10.0f;

	//static float count = 0.0f;
	//count = count + dt * scale;

	//control->UpdatePosition(_pos + Vector3(distScale * cos(Math::Common::Deg2Rad(count)), 0.0f, 0.0f));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}