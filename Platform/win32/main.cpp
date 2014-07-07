#include "DeviceDirector.h"

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	Device::DeviceDirector::WindowsInitOption options(hInst);
	Device::DeviceDirector::GetInstance()->Initialize(PresentInterval::Default, options);
//	Device::DeviceDirector::GetInstance()->SetScene(new TestScene);

	Device::DeviceDirector::GetInstance()->Run();
	Device::DeviceDirector::GetInstance()->GetApplication()->Destroy();
}
