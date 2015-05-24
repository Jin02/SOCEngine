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
	img->Create(Math::Size<uint>(538, 405));
	img->GetTransform()->UpdatePosition(Math::Vector3(0, 0, 0));
	
	Texture::Texture* texture = _textureMgr->LoadTextureFromFile("./Resource/test.png", false);

	img->UpdateMainImage(texture);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(mouse.GetLeft() == MOUSE::Action::Up)
	{
		int a = 5;
		a=3;
	}

	if(keyboard.states['A'] == KEYBOARD::Type::Up)
	{
		int a = 5;
		a=3;
	}
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