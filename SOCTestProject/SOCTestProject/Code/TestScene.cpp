#include "TestScene.h"
#include "BasicGeometryGenerator.h"

using namespace Core;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Geometry;

RenderSetting TestScene::RegistRenderSetting(Engine& engine)
{
	engine.GetRenderingSystem().GetPostProcessPipeline().SetUseDoF(true);
	engine.GetRenderingSystem().GetPostProcessPipeline().SetUseSSAO(true);

	GIInitParam param;
	{
		param.vxgiParam = GIInitParam::VXGIParam(256, 15.0f);
	}
	return RenderSetting("MainCamera", engine.GetDirectX().GetBackBufferRect().Cast<uint>(), 512, param);
}

void TestScene::OnInitialize(Engine& engine)
{
	Object* mainCamObj = engine.GetObjectManager().Find("MainCamera");
	assert(mainCamObj);
	MainCamera* mainCam = mainCamObj->GetComponent<MainCamera>();
	MainCamera& mainCam2 = engine.GetComponentSystem().GetMainCamera();

	// Load SkyBox
	{
		auto cubeMap = engine.GetRenderingSystem().GetTexture2DManager().LoadTextureFromFile(engine.GetDirectX(), "Resources/CubeMap/desertcube1024.dds", false);
		assert(cubeMap);

		Material::SkyBoxMaterial skyMat("@SkyBox");
		skyMat.UpdateCubeMap(*cubeMap);
		skyMat.Initialize(engine.GetDirectX(), engine.GetRenderingSystem().GetShaderManager());

		auto key = engine.GetRenderingSystem().GetMaterialManager().Add(skyMat.GetName(), skyMat).first;
		mainCam2.SetSkyBoxMaterialID(key);
	}

//	Object sphere = engine.GetObjectManager().Add("Sphere");
//	BasicGeometryGenerator::CreateBox(sphere, engine, Vector3(1.0f, 1.0f, 1.0f), uint(DefaultVertexInputTypeFlag::UV0) | uint(DefaultVertexInputTypeFlag::NORMAL));
//	engine.AddRootObject(sphere);

	Object* box = engine.LoadMesh("./Resources/CornellBox/box.obj"); assert(box);
	engine.AddRootObject(*box);

	Transform& tf = box->FetchTransform();
	tf.SetLocalPosition(Vector3(0.3f, -4.7f, 17.7f));
	tf.UpdateEulerAngle(Vector3(-90.0f, 0.0f, 180.0f));
	tf.SetLocalScale(Vector3(5.0f, 5.0f, 5.0f));

	Object& light = engine.GetObjectManager().Add("PointLight");
	light.FetchTransform().SetLocalPosition(Vector3(0.0f, 2.0f, 16.0f));

	PointLight& pl = light.AddComponent<PointLight>();
	pl.GetBase()->SetRadius(30.0f);
	pl.GetBase()->SetLumen(100);

	PointLightShadow& pls = light.AddComponent<PointLightShadow>();
	pls.GetBase()->SetUnderScanSize( Half(1.0f) );

	engine.AddRootObject(light);
	engine.UpdateWorldMatrix();

	auto v = box->FetchTransform().GetWorldPosition() + Vector3(0, 5, 0);
	engine.GetRenderingSystem().GetMainRenderer().GetGlobalIllumination().SetVXGI_VoxelizeCenterPos(v);
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
