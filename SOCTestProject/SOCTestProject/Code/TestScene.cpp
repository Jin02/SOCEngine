#include "TestScene.h"
#include "BasicGeometryGenerator.h"

//#define GI_TEST
//#define SKYBOX_ON

using namespace Core;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Geometry;

RenderSetting TestScene::RegistRenderSetting(Engine& engine)
{
//	engine.GetRenderingSystem().GetPostProcessPipeline().SetUseDoF(true);
//	engine.GetRenderingSystem().GetPostProcessPipeline().SetUseSSAO(false);
//	engine.GetRenderingSystem().GetPostProcessPipeline().SetUseSunShaft(false);

	GIInitParam param;
	{
#ifdef GI_TEST
		param.vxgiParam = GIInitParam::VXGIParam(256, 15.0f);
#else
		param.vxgiParam = GIInitParam::VXGIParam(0, 0);
#endif
	}
	return RenderSetting("MainCamera", engine.GetDirectX().GetBackBufferRect().Cast<uint>(), 2048, param);
}

void TestScene::OnInitialize(Engine& engine)
{
#ifdef SKYBOX_ON
	// Load SkyBox
	{
		auto cubeMap = engine.GetRenderingSystem().GetTexture2DManager().LoadTextureFromFile(engine.GetDirectX(), "Resources/CubeMap/desertcube1024.dds", false);
		assert(cubeMap);

		Material::SkyBoxMaterial skyMat("@SkyBox");
		skyMat.UpdateCubeMap(*cubeMap);
		skyMat.Initialize(engine.GetDirectX(), engine.GetRenderingSystem().GetShaderManager());

		auto key = engine.GetRenderingSystem().GetMaterialManager().Add(skyMat).first;
		engine.GetObjectManager().Find("MainCamera")->GetComponent<MainCamera>()->SetSkyBoxMaterialID(key);
	}
#endif

	constexpr uint defaultFlag = uint(DefaultVertexInputTypeFlag::UV0) | uint(DefaultVertexInputTypeFlag::NORMAL);

#if 0
	Object* box = engine.LoadMesh("./Resources/CornellBox/box.obj"); assert(box);
	engine.AddRootObject(*box);

	Transform& tf = box->FetchTransform();
	tf.SetLocalPosition(Vector3(0.3f, -4.7f, 17.7f));
	tf.UpdateLocalEulerAngle(Vector3(-90.0f, 0.0f, 180.0f));
	tf.SetLocalScale(Vector3(5.0f, 5.0f, 5.0f));

	Object& light = engine.GetObjectManager().Acquire("PointLight");
	light.FetchTransform().SetLocalPosition(Vector3(0.0f, 2.0f, 16.0f));

	PointLight& pl = light.AddComponent<PointLight>();
	pl.GetBase()->SetRadius(30.0f);
	pl.GetBase()->SetLumen(100);

	PointLightShadow& pls = light.AddComponent<PointLightShadow>();
	pls.GetBase()->SetUnderScanSize( Half(1.0f) );
	pls.GetBase()->SetBias(0.001f);

	engine.AddRootObject(light);

	engine.UpdateWorldMatrix();

	auto v = box->FetchTransform().GetWorldPosition() + Vector3(0, 5, 0);
	engine.GetRenderingSystem().GetMainRenderer().GetGlobalIllumination().SetVXGI_VoxelizeCenterPos(v);
#elif 0
	Material::PhysicallyBasedMaterial defaultMaterial("Default White Material");
	defaultMaterial.Initialize(engine.GetDirectX());
	auto materialKey = engine.GetRenderingSystem().GetMaterialManager().Add(defaultMaterial).first;

#ifdef 1
	Object* house = engine.LoadMesh("./Resources/House/SanFranciscoHouse.fbx"); assert(house);
	{
		engine.AddRootObject(*house);
	
		Transform& tf = house->FetchTransform();
		tf.SetLocalPosition(Vector3(0, -5, 20));
		tf.UpdateLocalEulerAngle(Vector3(-90, -90, 0));
	}
#else
	Object box = engine.GetObjectManager().Acquire("Box");
	{
		BasicGeometryGenerator::CreateBox(box, engine, Vector3(1, 1, 1), defaultFlag);
		engine.AddRootObject(box);

		box.FetchTransform().SetLocalPosition(Vector3(0, 0, 20));
		box.FetchTransform().SetLocalScale(Vector3(2, 10, 2));
		box.FetchTransform().UpdateLocalEulerAngle(Vector3(-90, -90, 0));

		auto mesh = box.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
#endif

	Object plane = engine.GetObjectManager().Acquire("Plane");
	{
		BasicGeometryGenerator::CreatePlane(plane, engine, 20.0f, 20.0f, 4, 4, defaultFlag);

		engine.AddRootObject(plane);

		plane.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));

		auto mesh = plane.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	Object plane2 = engine.GetObjectManager().Acquire("Plane2");
	{
		BasicGeometryGenerator::CreatePlane(plane2, engine, 20.0f, 20.0f, 4, 4, defaultFlag);

		engine.AddRootObject(plane2);

		plane2.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane2.FetchTransform().UpdateLocalEulerAngle(Vector3(180, 0, 0));

		auto mesh = plane2.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}

	Object& light = engine.GetObjectManager().Acquire("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(120.0f, 30.0f, 0.0f));

		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(20.0f);
		light.AddComponent<DirectionalLightShadow>().GetBase()->SetProjNear(30.0f);;

		engine.GetRenderingSystem().GetPostProcessPipeline().SetSunShaftParam(light.GetObjectID(), 0.2f, 30.0f);
		engine.AddRootObject(light);
	}

	auto mainCamObj = engine.GetObjectManager().Find("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5));

#ifndef SKYBOX_ON
	MaterialID matID = engine.ActivateSkyScattering(512, light);
	mainCamObj->GetComponent<MainCamera>()->SetSkyBoxMaterialID(matID);
#endif

#elif 0
	Material::PhysicallyBasedMaterial defaultMaterial("Default");
	defaultMaterial.Initialize(engine.GetDirectX());

	// Setting Material
	{
		auto& texMgr		= engine.GetRenderingSystem().GetTexture2DManager();
//		auto albedoMap		= texMgr.LoadTextureFromFile(engine.GetDirectX(), "./Resources/test.png", false);

		auto albedoMap		= texMgr.LoadTextureFromFile(engine.GetDirectX(), "./Resources/Rust/rustediron2_basecolor.png", false);
		auto metallicMap	= texMgr.LoadTextureFromFile(engine.GetDirectX(), "./Resources/Rust/rustediron2_metallic.png", false);
		auto roughnessMap	= texMgr.LoadTextureFromFile(engine.GetDirectX(), "./Resources/Rust/rustediron2_roughness.png", false);

		auto normalMap		= texMgr.LoadTextureFromFile(engine.GetDirectX(), "./Resources/Rust/rustediron2_normal.png", false);

		defaultMaterial.RegistDiffuseMap(*albedoMap);
		defaultMaterial.RegistMetallicMap(*metallicMap);
		defaultMaterial.RegistRoughnessMap(*roughnessMap);
		defaultMaterial.RegistNormalMap(*normalMap);
	}

	auto materialKey = engine.GetRenderingSystem().GetMaterialManager().Add(defaultMaterial).first;

	Object sphere = engine.GetObjectManager().Acquire("Sphere");
	{
		BasicGeometryGenerator::CreateSphere(sphere, engine, 8.0f, 32, 32, defaultFlag);
//		BasicGeometryGenerator::CreateBox(sphere, engine, Vector3(5.0f, 5.0f, 5.0f), defaultFlag);

		engine.AddRootObject(sphere);

		sphere.FetchTransform().SetLocalPosition(Vector3(0, 0, 20));
		sphere.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));

		auto mesh = sphere.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}

	Object& light = engine.GetObjectManager().Acquire("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(50.0f, 30.0f, 0.0f));

		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(1.0f);
		engine.AddRootObject(light);
	}

	auto mainCamObj = engine.GetObjectManager().Find("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5)); //-5

#ifndef SKYBOX_ON
	MaterialID matID = engine.ActivateSkyScattering(512, light);
	mainCamObj->GetComponent<MainCamera>()->SetSkyBoxMaterialID(matID);
#endif


#elif 1
	Material::PhysicallyBasedMaterial defaultMaterial("Default White Material");
	defaultMaterial.Initialize(engine.GetDirectX());
	defaultMaterial.SetReciveIBLMinRate(1.0f);
	auto materialKey = engine.GetRenderingSystem().GetMaterialManager().Add(defaultMaterial).first;

	Object cube = engine.GetObjectManager().Acquire("Cube");
	{
		BasicGeometryGenerator::CreateBox(cube, engine, Vector3(1, 1, 1), defaultFlag);
		engine.AddRootObject(cube);

		cube.FetchTransform().SetLocalPosition(Vector3(-7, 0, 20));
		cube.FetchTransform().SetLocalScale(Vector3(6, 6, 6));
		cube.FetchTransform().UpdateLocalEulerAngle(Vector3(-40, -45, 0));

		auto mesh = cube.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	Object cube2 = engine.GetObjectManager().Acquire("Cube2");
	{
		Material::PhysicallyBasedMaterial tMat("Default White Transparent Material");
		tMat.Initialize(engine.GetDirectX());
		tMat.SetMainColor(Color(1, 1, 1, 0.5f));
		tMat.SetReciveIBLMinRate(1.0f);
		auto tmk = engine.GetRenderingSystem().GetMaterialManager().Add(tMat).first;

		BasicGeometryGenerator::CreateBox(cube2, engine, Vector3(1, 1, 1), defaultFlag);
		engine.AddRootObject(cube2);

		cube2.FetchTransform().SetLocalPosition(Vector3(7, 0, 20));
		cube2.FetchTransform().SetLocalScale(Vector3(6, 6, 6));
		cube2.FetchTransform().UpdateLocalEulerAngle(Vector3(-40, -45, 0));

		auto& meshMgr = engine.GetComponentSystem().GetManager<Mesh>();
		meshMgr.ChangeTrait(cube2.GetObjectID(), *meshMgr.GetOpaqueMeshPool(), *meshMgr.GetTransparentMeshPool());
		cube2.GetComponent<Mesh>()->SetPBRMaterialID(tmk);
	}

	Object plane2 = engine.GetObjectManager().Acquire("Plane");
	{
		BasicGeometryGenerator::CreatePlane(plane2, engine, 20.0f, 20.0f, 4, 4, defaultFlag);
	
		engine.AddRootObject(plane2);
	
		plane2.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane2.FetchTransform().UpdateLocalEulerAngle(Vector3(0, 0, 0));
	
		auto mesh = plane2.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(materialKey);
	}
	
	Object& light = engine.GetObjectManager().Acquire("Light");
	{
		light.FetchTransform().UpdateLocalEulerAngle(Vector3(120.0f, 30.0f, 0.0f));
	
		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(20.0f);
		light.AddComponent<DirectionalLightShadow>().GetBase()->SetProjNear(30.0f);;
	
		engine.GetRenderingSystem().GetPostProcessPipeline().SetSunShaftParam(light.GetObjectID(), 0.2f, 30.0f);
		engine.AddRootObject(light);
	}
	
	auto mainCamObj = engine.GetObjectManager().Find("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5));

#ifndef SKYBOX_ON
	MaterialID matID = engine.ActivateSkyScattering(512, light);
	mainCamObj->GetComponent<MainCamera>()->SetSkyBoxMaterialID(matID);
#endif

#endif

}

void TestScene::OnDestroy(Engine&)
{
}

void TestScene::OnRenderPreview(Engine&)
{
}

void TestScene::OnUpdate(Engine& engine)
{
	//static constexpr float rate = 0.16f;

	//auto& tf = engine.GetObjectManager().Find("Light")->FetchTransform();
	//Vector3 euler = tf.GetLocalEularAngle();
	//tf.UpdateLocalEulerAngle(euler + Vector3(rate, 0, 0));
}

void TestScene::OnRenderPost(Engine&)
{
}

void TestScene::OnInput(Engine&)
{
}
