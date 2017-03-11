#include "WinApp.h"

using namespace Device;

LRESULT WinApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// TODO
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

WinApp::WinApp(const Rect<uint> &rect, HINSTANCE Instance, const std::string& name, bool windowMode, bool isChild, HWND parentHandle)
{
	_windowInfo.cbSize			= sizeof(WNDCLASSEX);
	_windowInfo.style			= CS_CLASSDC;
	_windowInfo.hInstance		= Instance;//GetModuleHandle(NULL);
	_windowInfo.cbClsExtra		= 0L;
	_windowInfo.cbWndExtra		= 0L;
	_windowInfo.hIcon			= NULL;
	_windowInfo.hCursor			= NULL;
	_windowInfo.hbrBackground	= NULL;
	_windowInfo.hIconSm			= NULL;
	_windowInfo.lpszMenuName	= NULL;
	_windowInfo.lpszClassName	= name.data();

	_windowInfo.lpfnWndProc = WndProc;

	_options				= isChild ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW | WS_SYSMENU;
	_rect					= rect;
	_parentHandle			= parentHandle;
	_windowsMode			= windowMode;
}

WinApp::~WinApp(void)
{
	Destroy();
}

bool WinApp::Initialize()
{
	RegisterClassEx(&_windowInfo);

	_handle = CreateWindow(_windowInfo.lpszClassName, _windowInfo.lpszClassName, _options,
		_rect.x, _rect.y,
		_rect.size.w, _rect.size.h,
		_parentHandle, NULL, _windowInfo.hInstance, NULL);

	if (_options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN))
		ShowWindow(_handle, SW_SHOWDEFAULT);

	return true;
}
void WinApp::Destroy()
{
	UnregisterClass(_windowInfo.lpszClassName, _windowInfo.hInstance);

	if (_options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN))
		DestroyWindow(_handle);
}