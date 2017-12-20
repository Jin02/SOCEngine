#include "TestScene.h"
#include "BasicGeometryGenerator.h"

//#define GI_TEST
#define SKYBOX_ON

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
//	engine.GetRenderingSystem().GetPostProcessPipeline().SetUseSSAO(true);

	GIInitParam param;
	{
#ifdef GI_TEST
		param.vxgiParam = GIInitParam::VXGIParam(256, 15.0f);
#else
		param.vxgiParam = GIInitParam::VXGIParam(0, 0);
#endif
	}
	return RenderSetting("MainCamera", engine.GetDirectX().GetBackBufferRect().Cast<uint>(), 512, param);
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

#ifdef GI_TEST
	Object* box = engine.LoadMesh("./Resources/CornellBox/box.obj"); assert(box);
	engine.AddRootObject(*box);

	Transform& tf = box->FetchTransform();
	tf.SetLocalPosition(Vector3(0.3f, -4.7f, 17.7f));
	tf.UpdateEulerAngle(Vector3(-90.0f, 0.0f, 180.0f));
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
#else
	Object* house = engine.LoadMesh("./Resources/House/SanFranciscoHouse.fbx"); assert(house);
	{
		engine.AddRootObject(*house);
	
		Transform& tf = house->FetchTransform();
		tf.SetLocalPosition(Vector3(0, -5, 20));
		tf.UpdateEulerAngle(Vector3(-90, -90, 0));
	}

	Object plane = engine.GetObjectManager().Acquire("Plane");
	{
		constexpr uint flag =	uint(DefaultVertexInputTypeFlag::UV0) |
								uint(DefaultVertexInputTypeFlag::NORMAL);
		BasicGeometryGenerator::CreatePlane(plane, engine, 20.0f, 20.0f, 4, 4, flag);

		engine.AddRootObject(plane);

		plane.FetchTransform().SetLocalPosition(Vector3(0, -5, 20));
		plane.FetchTransform().UpdateEulerAngle(Vector3(0, 0, 0));

		Material::PhysicallyBasedMaterial pbr("PlaneMaterial");
		pbr.Initialize(engine.GetDirectX());
		auto key = engine.GetRenderingSystem().GetMaterialManager().Add(pbr).first;
		auto mesh = plane.GetComponent<Mesh>();
		mesh->SetPBRMaterialID(key);
	}

	Object& light = engine.GetObjectManager().Acquire("Light");
	{
		light.FetchTransform().SetLocalPosition(Vector3(0, 0, 0));
		light.FetchTransform().UpdateEulerAngle(Vector3(120.0f, 30.0f, 0.0f));
	
		light.AddComponent<DirectionalLight>().GetBase()->SetIntensity(20.0f);
		light.AddComponent<DirectionalLightShadow>();
	
		engine.AddRootObject(light);
	}

	auto mainCamObj = engine.GetObjectManager().Find("MainCamera");
	mainCamObj->FetchTransform().SetLocalPosition(Vector3(0,0,-5));
#endif
}

void TestScene::OnDestroy(Engine&)
{
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnUpdate()
{
}

void TestScene::OnRenderPost()
{
}

void TestScene::OnInput()
{
}
