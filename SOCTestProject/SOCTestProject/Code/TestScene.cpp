#include "TestScene.h"

using namespace Core;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Light;
using namespace Rendering::Shadow;

RenderSetting TestScene::RegistRenderSetting(Engine& engine)
{
	return RenderSetting("MainCamera",
		engine.GetDirectX().GetBackBufferRect().Cast<uint>(),
		1024, 256, 15.0f
	);
}

void TestScene::OnInitialize(Engine& engine)
{
	Object* mainCamObj = engine.GetObjectManager().Find("MainCamera");
	assert(mainCamObj);
	bool hasMainCam = mainCamObj->HasComponent<MainCamera>();
	assert(hasMainCam); // ????

	Object* box = engine.LoadMesh("./Resources/CornellBox/box.obj");
	assert(box);

	Transform& tf = box->FetchTransform();
	tf.UpdatePosition(Vector3(0.3f, -4.7f, 17.7f));
	tf.UpdateEulerAngle(Vector3(-90.0f, 0.0f, 180.0f));
	tf.UpdateScale(Vector3(5.0f, 5.0f, 5.0f));
	engine.AddRootObject(*box);

	Object& light = engine.GetObjectManager().Add("PointLight");
	light.FetchTransform().UpdatePosition(Vector3(0.0f, 2.0f, 16.0f));

	PointLight& pl = light.AddComponent<PointLight>();
	pl.GetBase().SetRadius(30.0f);
	pl.GetBase().SetLumen(100);

	PointLightShadow& pls = light.AddComponent<PointLightShadow>();
	pls.GetBase().SetUnderScanSize( Half(0.0f) );

	engine.AddRootObject(light);
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
