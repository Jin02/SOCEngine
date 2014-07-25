//#include "DeviceDirector.h"

#include <Windows.h>
#include "Structure.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
//	Device::DeviceDirector::WindowsInitOption options(hInst);
//	Device::DeviceDirector::GetInstance()->Initialize(PresentInterval::Default, options);
////	Device::DeviceDirector::GetInstance()->SetScene(new TestScene);
//
//	Device::DeviceDirector::GetInstance()->Run();
//	Device::DeviceDirector::GetInstance()->GetApplication()->Destroy();

	{
		Structure::Vector<int> p;
		int a = 5;
		p.Add("Test", &a, true);
		p.Find("test");
		p.Delete("test");
		p.DeleteAll();
	}

	{
		Structure::Map<int> p;
		int a = 5;
		p.Add("Test", &a, true);
		p.Find("test");
		p.Delete("test");
		p.DeleteAll();
	}

	{
		Structure::HashMap<int> p;
		int a = 5;
		p.Add("Test", &a, true);
		p.Find("test");
		p.Delete("test");
		p.DeleteAll();
	}

}

void te(int* const ttt)
{

}