#include <Windows.h>
#include <Director.h>
#include "TestScene.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	auto rect = Math::Rect<unsigned int>(0, 0, 1600, 900);
	Device::Director::SharedInstance()->Initialize(rect, rect, hInst, "Test", true, false, 0, false);
	Device::Director::SharedInstance()->SetCurrentScene(new TestScene);
	Device::Director::SharedInstance()->Run();
	Device::Director::Destroy();

	return 0;
}