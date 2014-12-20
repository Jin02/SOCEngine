#include <Windows.h>
#include "Director.h"
#include "TestScene.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	Device::Director::GetInstance()->Initialize(Math::Rect<unsigned int>(0, 0, 640, 480), hInst, "Test", true, false, 0);
	Device::Director::GetInstance()->SetCurrentScene(new TestScene);
	Device::Director::GetInstance()->Run();
	Device::Director::GetInstance()->Destroy();
}