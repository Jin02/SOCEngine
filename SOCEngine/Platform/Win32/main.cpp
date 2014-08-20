#include "Director.h"
#include "TestScene.h"
#include "ObjImporter.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	Importer::ObjImporter o;
	o.Load("../../../Test/crysponza/", "sponza.obj");

	Device::Director::GetInstance()->Initialize(Math::Rect<int>(0, 0, 640, 480), hInst, "Test", true, false, 0);
	Device::Director::GetInstance()->SetCurrentScene(new TestScene);
	Device::Director::GetInstance()->Run();
	Device::Director::GetInstance()->Destroy();
}