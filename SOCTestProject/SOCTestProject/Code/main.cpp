#include <Windows.h>
#include "Director.h"
#include "TestScene.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	auto rect = Math::Rect<unsigned int>(0, 0, 1280, 800);
	Device::Director::GetInstance()->Initialize(rect, rect, hInst, "Test", true, false, 0);
	Device::Director::GetInstance()->SetCurrentScene(new TestScene);
	Device::Director::GetInstance()->Run();
	Device::Director::GetInstance()->Destroy();
}