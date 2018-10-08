#include "TestScene.h"
#include "BasicGeometryGenerator.h"
#include <EngineUtility.h>

//#define GI_TEST
#define SKYBOX_ON
//#define SKY_SCATTERING
//#define SHADOW_TEST
//#define SHADOW_TEST_USE_HOUSE
//#define PBR_TEST
//#define TRANSPARENT_TEST
#define MOTION_BLUR_TEST

using namespace Core;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Geometry;

RenderSetting TestScene::RegistRenderSetting(Engine& engine, EngineUtility& util)
{
//	util.SetUseDoF(true);
//	util.SetUseSSAO(false);
//	util.SetUseSunShaft(false);
	util.SetUseMotionBlur(true);
	engine.SetUpdateTime(1);

	GIInitParam param;
	{
#ifdef GI_TEST
		param.vxgiParam = GIInitParam::VXGIParam(256, 15.0f);
#else
		param.vxgiParam = GIInitParam::VXGIParam(0, 0);
#endif
	}
	return RenderSetting("MainCamera", engine.GetDirectX().GetBackBufferRect().Cast<uint>(), 2048, param, false);
}

void TestScene::OnInitialize(Engine& engine, EngineUtility& util)
{
	auto test = util.LoadMesh("Resources/Deer/Deer2_model.fbx");
	assert(0);

#ifdef SKYBOX_ON
	util.ActivateSkyBox("Resources/CubeMap/desertcube1024.dds", "@SkyBox");
#endif

#ifdef GI_TEST
	Object* box = util.LoadMesh("./Resources/CornellBox/box.obj"); assert(box);
	engine.AddRootObject(*box);

	Transform& tf = box->FetchTransform();
	tf.SetLocalPosition(Vector3(0.3f, -4.7f, 17.7f));
	tf.UpdateLocalEulerAngle(Vector3(-90.0f, 0.0f, 180.0f));
	tf.SetLocalScale(Vector3(5.0f, 5.0f, 5.0f));

	Object& light = util.AcquireObject("PointLight");
	light.FetchTransform().SetLocalPosition(Vector3(0.0f, 2.0f, 16.0f));

	PointLight& pl = light.AddComponent<PointLight>();
	pl.GetBase()->SetRadius(30.0f);
	pl.GetBase()->SetLumen(100);

	PointLightShadow& pls = light.AddComponent<PointLightShadow>();
	pls.GetBase()->SetUnderScanSize( Half(1.0f) );
	pls.GetBase()->SetBias(0.001f);

	engine.AddRootObject(light);
	engine.UpdateWorldMatrix();

	util.SetVXGICenterPosition(box->FetchTransform().GetWorldPosition() + Vector3(0, 5, 0));
#elif defined(SHADOW_TEST)
	auto materialKey = util.AddMaterialToPool(util.AcquireMaterial("Default White Material"));


#ifdef SHADOW_TEST_USE_HOUSE
	Object* house = util.LoadMesh("./Resources/House/SanFranciscoHouse.fbx"); assert(house);
	{
		engine.AddRootObject(*house);
	
		Transform& tf = house->FetchTransform();
		tf.SetLocalPosition(Vector3(0, -5, 20));
		tf.UpdateLocalEulerAngle(Vector3(-90, -90, 0));
	}
#else
	Object box = engine.GetObjectManager().Acquire("Box");
	{
		BasicGeometryGenerator::CreateBox(box, engine, Vector3(1, 1, 1));
		engine.AddRootObject(box);

		box.FetchTransform().SetLocalPosition(Vector3(0, 0, 20));
		box.FetchTransform().SetLocalScale(Vector3(2, 10, 2));
		box.FetchTransform().UpdateLocalEulerAngle(Vector3(-90, -90, 0));

		auto mesh = box.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
#endif

	Object plane = util.AcquireObject("Plane");
	{
		BasicGeometryGenerator::CreatePlane(plane, engine, 20.0f, 20.0f, 4, 4);

		engine.AddRootObject(plane);

		plane.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));

		auto mesh = plane.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	Object plane2 = util.AcquireObject("Plane2");
	{
		BasicGeometryGenerator::CreatePlane(plane2, engine, 20.0f, 20.0f, 4, 4);

		engine.AddRootObject(plane2);

		plane2.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane2.FetchTransform().UpdateLocalEulerAngle(Vector3(180, 0, 0));

		auto mesh = plane2.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}

	Object& light = util.AcquireObject("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(120.0f, 30.0f, 0.0f));

		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(20.0f);
		light.AddComponent<DirectionalLightShadow>().GetBase()->SetProjNear(30.0f);;

		util.SetSunShaftParam(light.GetObjectID(), 0.2f, 30.0f);
		engine.AddRootObject(light);
	}

	auto mainCamObj = util.FindObject("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5));

#ifdef SKY_SCATTERING
	util.ActivateSkyScattering(512, light);
#endif

#elif defined(PBR_TEST)
	Material::PhysicallyBasedMaterial defaultMaterial = util.AcquireMaterial("Default");

	// Setting Material
	{
		auto albedoMap		= util.LoadTextureFromFile("./Resources/Rust/rustediron2_basecolor.png", false);
		auto metallicMap	= util.LoadTextureFromFile("./Resources/Rust/rustediron2_metallic.png", false);
		auto roughnessMap	= util.LoadTextureFromFile("./Resources/Rust/rustediron2_roughness.png", false);

		auto normalMap		= util.LoadTextureFromFile("./Resources/Rust/rustediron2_normal.png", false);

		defaultMaterial.RegistDiffuseMap(*albedoMap);
		defaultMaterial.RegistMetallicMap(*metallicMap);
		defaultMaterial.RegistRoughnessMap(*roughnessMap);
		defaultMaterial.RegistNormalMap(*normalMap);
	}

	auto materialKey = util.AddMaterialToPool(defaultMaterial);

	Object sphere = util.AcquireObject("Sphere");
	{
		BasicGeometryGenerator::CreateSphere(sphere, engine, 8.0f, 32, 32);
//		BasicGeometryGenerator::CreateBox(sphere, engine, Vector3(5.0f, 5.0f, 5.0f));

		engine.AddRootObject(sphere);

		sphere.FetchTransform().SetLocalPosition(Vector3(0, 0, 20));
		sphere.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));

		auto mesh = sphere.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}

	Object& light = util.AcquireObject("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(50.0f, 30.0f, 0.0f));

		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(1.0f);
		engine.AddRootObject(light);
	}

	auto mainCamObj = util.FindObject("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5)); //-5

#ifdef SKY_SCATTERING
	util.ActivateSkyScattering(512, light);
#endif

#elif defined(TRANSPARENT_TEST)
	Material::PhysicallyBasedMaterial defaultMaterial = util.AcquireMaterial("Default White Material");
	defaultMaterial.SetReciveIBLMinRate(1.0f);
	auto materialKey = util.AddMaterialToPool(defaultMaterial);

	Object cube = util.AcquireObject("Cube");
	{
		BasicGeometryGenerator::CreateBox(cube, engine, Vector3(1, 1, 1));
		engine.AddRootObject(cube);

		cube.FetchTransform().SetLocalPosition(Vector3(-7, 0, 20));
		cube.FetchTransform().SetLocalScale(Vector3(6, 6, 6));
		cube.FetchTransform().UpdateLocalEulerAngle(Vector3(-40, -45, 0));

		auto mesh = cube.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	Object cube2 = util.AcquireObject("Cube2");
	{
		Material::PhysicallyBasedMaterial tMat("Default White Transparent Material");
		tMat.Initialize(engine.GetDirectX());
		tMat.SetMainColor(Color(1, 1, 1, 0.5f));
		tMat.SetReciveIBLMinRate(1.0f);
		auto tmk = util.AddMaterialToPool(tMat);

		BasicGeometryGenerator::CreateBox(cube2, engine, Vector3(1, 1, 1));
		engine.AddRootObject(cube2);

		cube2.FetchTransform().SetLocalPosition(Vector3(7, 0, 20));
		cube2.FetchTransform().SetLocalScale(Vector3(6, 6, 6));
		cube2.FetchTransform().UpdateLocalEulerAngle(Vector3(-40, -45, 0));

		util.OpaqueMeshToTransparentMesh(cube2.GetObjectID());
		cube2.GetComponent<Mesh>()->SetPBRMaterialID(tmk);
	}

	Object plane2 = util.AcquireObject("Plane");
	{
		BasicGeometryGenerator::CreatePlane(plane2, engine, 20.0f, 20.0f, 4, 4);
	
		engine.AddRootObject(plane2);
	
		plane2.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane2.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));
	
		auto mesh = plane2.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	
	Object& light = util.AcquireObject("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(120.0f, 30.0f, 0.0f));
	
		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(20.0f);
		light.AddComponent<DirectionalLightShadow>().GetBase()->SetProjNear(30.0f);;
	
		util.SetSunShaftParam(light.GetObjectID(), 0.2f, 30.0f);
		engine.AddRootObject(light);
	}
	
	auto mainCamObj = util.FindObject("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5));

#ifdef SKY_SCATTERING
	util.ActivateSkyScattering(512, light);
#endif

#elif defined(MOTION_BLUR_TEST)
	Material::PhysicallyBasedMaterial defaultMaterial = util.AcquireMaterial("Default White Material");
	defaultMaterial.SetReciveIBLMinRate(1.0f);
	auto materialKey = util.AddMaterialToPool(defaultMaterial);
	
	Object cube = util.AcquireObject("Cube");
	{
		BasicGeometryGenerator::CreateBox(cube, engine, Vector3(1, 1, 1));
		engine.AddRootObject(cube);

		_objectOriginPos = Vector3(-7, 3, 20);
		cube.FetchTransform().SetLocalPosition(Vector3(-7, 0, 20));
		cube.FetchTransform().SetLocalScale(Vector3(6, 6, 6));
		cube.FetchTransform().UpdateLocalEulerAngle(Vector3(-40, -45, 0));
	
		auto mesh = cube.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	Object cube2 = util.AcquireObject("Cube2");
	{
		BasicGeometryGenerator::CreateBox(cube2, engine, Vector3(1, 1, 1));
		engine.AddRootObject(cube2);
	
		_object2OriginPos = Vector3(7, 3, 20);
		cube2.FetchTransform().SetLocalPosition(Vector3(7, 0, 20));
		cube2.FetchTransform().SetLocalScale(Vector3(6, 6, 6));
		cube2.FetchTransform().UpdateLocalEulerAngle(Vector3(-40, -45, 0));
	
		auto mesh = cube2.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	
	Object plane2 = util.AcquireObject("Plane");
	{
		BasicGeometryGenerator::CreatePlane(plane2, engine, 20.0f, 20.0f, 4, 4);
	
		engine.AddRootObject(plane2);
	
		plane2.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane2.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));
	
		auto mesh = plane2.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	
	Object& light = util.AcquireObject("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(120.0f, 30.0f, 0.0f));
	
		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(20.0f);
		light.AddComponent<DirectionalLightShadow>().GetBase()->SetProjNear(30.0f);;
	
		util.SetSunShaftParam(light.GetObjectID(), 0.2f, 30.0f);
		engine.AddRootObject(light);
	}
	
	auto mainCamObj = util.FindObject("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5));

#ifdef SKY_SCATTERING
	util.ActivateSkyScattering(512, light);
#endif

#endif

}

void TestScene::OnDestroy(Engine&, EngineUtility&)
{
}

void TestScene::OnRenderPreview(Engine&, EngineUtility&)
{
}

void TestScene::OnUpdate(Engine& engine, EngineUtility& util)
{
#ifdef MOTION_BLUR_TEST
	auto Move = [&engine](const std::string& name, const Vector3& move)
	{
		auto& tf = engine.GetObjectManager().Find(name)->FetchTransform();
		tf.SetLocalPosition(move);
	};

	static float rate = 0.0f;
	Move("Cube",	_objectOriginPos + Vector3(0, sin(rate) * 2.0f, 0));
	Move("Cube2",	_object2OriginPos + Vector3(0, cos(rate) * 2.0f, 0));

	rate += MATH_PI * 0.25f / 180.0f;

#else
	//float rate = 0.001f;

	//auto& tf = engine.GetObjectManager().Find("MainCamera")->FetchTransform();
	//Vector3 euler = tf.GetLocalEularAngle();
	//tf.UpdateLocalEulerAngle(euler + Vector3(0, rate, 0));
#endif
}

void TestScene::OnRenderPost(Engine&, EngineUtility&)
{
}

void TestScene::OnInput(Engine&, EngineUtility&)
{
}
