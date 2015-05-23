#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"

using namespace Rendering;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	img = new UI::SimpleImage2D("Test");
	img->Create(Math::Size<uint>(100, 100));
	img->GetTransform()->UpdatePosition(Math::Vector3(350, 0, 0));
	
	Texture::Texture* texture = _textureMgr->LoadTextureFromFile("./Resource/test.png", false);

	img->UpdateMainImage(texture);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnUpdate(float dt)
{
	//static float x = 0.0;
	//x+= 0.001f;
	//img->GetTransform()->UpdatePosition(Math::Vector3(0, 0, x));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}