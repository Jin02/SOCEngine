#include "WinApp.h"
#include "Launcher.h"

using namespace Device;

LRESULT WinApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// TODO
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

WinApp::WinApp(const Param& param)
{
	_windowInfo.cbSize			= sizeof(WNDCLASSEX);
	_windowInfo.style			= CS_CLASSDC;
	_windowInfo.hInstance		= param.Instance;//GetModuleHandle(NULL);
	_windowInfo.cbClsExtra		= 0L;
	_windowInfo.cbWndExtra		= 0L;
	_windowInfo.hIcon			= NULL;
	_windowInfo.hCursor			= NULL;
	_windowInfo.hbrBackground	= NULL;
	_windowInfo.hIconSm			= NULL;
	_windowInfo.lpszMenuName	= NULL;
	_windowInfo.lpszClassName	= param.name.data();

	_windowInfo.lpfnWndProc = WndProc;

	_options				= param.isChild ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW | WS_SYSMENU;
	_rect					= param.rect;
	_parentHandle			= param.parentHandle;
	_windowsMode			= param.windowMode;
}

void WinApp::Initialize()
{
	RegisterClassEx(&_windowInfo);

	_handle = CreateWindow(_windowInfo.lpszClassName, _windowInfo.lpszClassName, _options,
		_rect.x, _rect.y,
		_rect.size.w, _rect.size.h,
		_parentHandle, NULL, _windowInfo.hInstance, NULL);

	if (_options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN))
		ShowWindow(_handle, SW_SHOWDEFAULT);
}
void WinApp::Destroy()
{
	UnregisterClass(_windowInfo.lpszClassName, _windowInfo.hInstance);

	if (_options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN))
		DestroyWindow(_handle);
}