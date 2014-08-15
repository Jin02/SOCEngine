#include "Director.h"
#include "TestScene.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	Structure::Map<int> m;
	m.GetSize();

	Structure::HashMap<int> h;
	h.GetSize();

	std::vector<std::string> t;
	Utility::Tokenize("\tNs 10.0000", t);

	std::vector<int> ttttt;
	ttttt.at(0);

	Device::Director::GetInstance()->Initialize(Math::Rect<int>(0, 0, 640, 480), hInst, "Test", true, false, 0);
	Device::Director::GetInstance()->SetCurrentScene(new TestScene);
	Device::Director::GetInstance()->Run();
	Device::Director::GetInstance()->Destroy();
}