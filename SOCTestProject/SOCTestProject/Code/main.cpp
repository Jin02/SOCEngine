#include "Launcher.h"
#include "TestScene.h"

using namespace Core;
using namespace Device;

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	WinApp::Desc desc;
	{
		desc.rect			= Rect<uint>(0, 0, 1600, 900);
		desc.instance		= hInst;
		desc.name			= "Refactoring";
		desc.windowMode		= true;
		desc.isChild		= false;
		desc.parentHandle	= NULL;
	}

	Launcher::Run(desc, desc.rect, false, new TestScene);
	return 0;
}