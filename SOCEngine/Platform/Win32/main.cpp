#include "Director.h"
#include "TestScene.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	Structure::Map<int> m;
	m.GetSize();

	Structure::HashMap<int> h;
	h.GetSize();

	Device::Director::GetInstance()->Initialize(Math::Rect<int>(0, 0, 800, 600), hInst, "Test", true, false, 0);
	Device::Director::GetInstance()->SetCurrentScene(new TestScene);
	Device::Director::GetInstance()->Run();
	Device::Director::GetInstance()->Destroy();
}