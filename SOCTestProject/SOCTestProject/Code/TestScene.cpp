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
//	ActivateGI(true, 256, 40.0f);

	// SkyBox
	ActiveSkyBox("@Skybox", "Resources/CubeMap/desertcube1024.dds");

	_camera = new Object("Default");
	MeshCamera* cam = _camera->AddComponent<MeshCamera>();
	_camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
//	_camera->GetTransform()->UpdatePosition(Vector3(0, -0.3f, -4.1f));
	cam->SetFieldOfViewDegree(60.0f);

	const ResourceManager* resourceMgr	= ResourceManager::SharedInstance();
	Importer::MeshImporter* importer	= resourceMgr->GetMeshImporter();

#if 0
	_testObject = importer->Load("./Resources/Capsule/capsule.obj");
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
	AddObject(_testObject);
#else
	BasicGeometryGenerator gen;
	uint flag = uint(RenderManager::DefaultVertexInputTypeFlag::UV0) |
				uint(RenderManager::DefaultVertexInputTypeFlag::NORMAL);
//	_testObject = gen.CreatePlane(0.5f, 0.5f, 4, 4, flag);
//	_testObject = gen.CreateCylinder(0.5f, 0.5f, 0.5f, 30, 30, flag);
//	_testObject = gen.CreateBox(Vector3(0.5f, 0.5f, 0.5f), flag);
	_testObject = gen.CreateSphere(1.0f, 30, 30, flag);
//	_testObject = importer->Load("./Resources/Sphere/sphere.obj", false);
	_testObject->GetTransform()->UpdatePosition(Vector3(0, 0.0f, 11.0f));
	_testObject->GetTransform()->UpdateEulerAngles(Vector3(0.0f, 0.0f, 0.0f));
	_testObject->GetTransform()->UpdateScale(Vector3(5, 5, 5));
	AddObject(_testObject);

	//_testObject = importer->Load("./Resources/House/SanFranciscoHouse.fbx");
	//_camera->GetTransform()->UpdatePosition(Vector3(0, 5, -15));
	//_testObject->GetTransform()->UpdateEulerAngles(Vector3(-90, -90, 0));

	//_testObject = importer->Load("./Resources/CornellBox/box.obj", false);
	//_testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	//_testObject->GetTransform()->UpdateEulerAngles(Vector3(-90, 180.0f, 180));
	//_testObject->GetTransform()->UpdateScale(Vector3(5, 5, 5));
	//AddObject(_testObject);

	//_testObject2 = importer->Load("./Resources/CornellBox/Sphere.obj", false);
	//_testObject2->GetTransform()->UpdatePosition(Vector3(0, -5.5f, 17.2f));
	//_testObject2->GetTransform()->UpdateEulerAngles(Vector3(-90.0f, 0.0f, 180.0f));
	//_testObject2->GetTransform()->UpdateScale(Vector3(10, 10, 10));
	//AddObject(_testObject2);
	//cam->SetFieldOfViewDegree(75.0f);

	//_testObject2 = importer->Load("./Resources/Voxel/voxel.obj", false);
	//_testObject2->GetTransform()->UpdatePosition(Vector3(-2.5f, 2.2f, 14.0f));
	//_testObject2->GetTransform()->UpdateEulerAngles(Vector3(0.0f, 0.0f, 0.0f));
	//_testObject2->GetTransform()->UpdateScale(Vector3(3, 3, 3));
	//AddObject(_testObject2);

	{
		//PhysicallyBasedMaterial* left	= static_cast<PhysicallyBasedMaterial*>(_testObject2->GetChild(0)->GetChild(3)->GetComponent<Rendering::Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
		//left->UpdateMainColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

		//PhysicallyBasedMaterial* right	= static_cast<PhysicallyBasedMaterial*>(_testObject2->GetChild(0)->GetChild(0)->GetComponent<Rendering::Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
		//right->UpdateMainColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

		//PhysicallyBasedMaterial* back	= static_cast<PhysicallyBasedMaterial*>(_testObject2->GetChild(0)->GetChild(1)->GetComponent<Rendering::Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
		//back->UpdateMainColor(Color(0.0f, 1.0f, 1.0f, 1.0f));

		//PhysicallyBasedMaterial* front	= static_cast<PhysicallyBasedMaterial*>(_testObject2->GetChild(0)->GetChild(4)->GetComponent<Rendering::Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
		//front->UpdateMainColor(Color(1.0f, 1.0f, 0.0f, 1.0f));

		//PhysicallyBasedMaterial* top	= static_cast<PhysicallyBasedMaterial*>(_testObject2->GetChild(0)->GetChild(5)->GetComponent<Rendering::Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
		//top->UpdateMainColor(Color(1.0f, 1.0f, 1.0f, 1.0f));

		//PhysicallyBasedMaterial* bottom	= static_cast<PhysicallyBasedMaterial*>(_testObject2->GetChild(0)->GetChild(2)->GetComponent<Rendering::Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
		//bottom->UpdateMainColor(Color(1.0f, 0.0f, 1.0f, 1.0f));
	}

#endif
	
#if 0
	_light = new Object("Light");
	//_light->GetTransform()->UpdateEulerAngles(Vector3(60, 60, 0));
	//_light->GetTransform()->UpdatePosition(Vector3(-1, 1.5,  12));
	_light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));
	_light->GetTransform()->UpdatePosition(Vector3(0.5f, -0.8, 6.0f));

	SpotLight* light = _light->AddComponent<SpotLight>();
	light->SetLumen(150);
	light->SetRadius(10.0f);
	light->ActiveShadow(true);
	light->SetSpotAngleDegree(120.0f);
	light->GetShadow()->SetUseVSM(false);
#elif 1
	_light = new Object("Light");
//	_light->GetTransform()->UpdateEulerAngles(Vector3(315, 340, 0));
//	_light->GetTransform()->UpdateEulerAngles(Vector3(90, 0, 0));
	_light->GetTransform()->UpdateEulerAngles(Vector3(90.0f, 0.0f, 0));

//	_light->GetTransform()->UpdatePosition(Vector3(0, 30, 0));
	_light->GetTransform()->UpdatePosition(_camera->GetTransform()->GetLocalPosition());

	DirectionalLight* light = _light->AddComponent<DirectionalLight>();
//	light->SetProjectionSize(10);
//	light->SetProjectionSize(30);
	light->SetIntensity(8.0f);
	light->SetUseAutoProjectionLocation(true);
	light->ActiveShadow(false);
#elif 0
	_light = new Object("Light");
//	_light->GetTransform()->UpdatePosition(Vector3(0.0f, 0.9f, 8.0f));
//	_light->GetTransform()->UpdatePosition(Vector3(0.0f, 3.4f, 10.0f));
	_light->GetTransform()->UpdatePosition(Vector3(0.0f, 5.2f, 12.0f));

	PointLight* light = _light->AddComponent<PointLight>();
	light->SetLumen(100);
	light->SetRadius(30.0f);
	light->ActiveShadow(true);
	light->GetShadow()->SetUnderScanSize(0.0f);
#endif

	AddObject(_light);
}

void TestScene::OnRenderPreview()
{
	if(_globalIllumination)
	{
		auto voxelViwer = _globalIllumination->GetDebugVoxelViewer();
		
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
	Transform* control = _camera->GetTransform();//_testObject2 ? _testObject2->GetTransform() : _camera->GetTransform();
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
		//_testObject->SetUse(!_testObject->GetUse());
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