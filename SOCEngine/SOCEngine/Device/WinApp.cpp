#include "WinApp.h"
#include "Launcher.h"

using namespace Device;

LRESULT WinApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

WinApp::WinApp(const Desc& desc)
{
	_windowInfo.cbSize			= sizeof(WNDCLASSEX);
	_windowInfo.style			= CS_CLASSDC;
	_windowInfo.hInstance		= desc.instance;//GetModuleHandle(NULL);
	_windowInfo.cbClsExtra		= 0L;
	_windowInfo.cbWndExtra		= 0L;
	_windowInfo.hIcon			= NULL;
	_windowInfo.hCursor			= NULL;
	_windowInfo.hbrBackground	= NULL;
	_windowInfo.hIconSm			= NULL;
	_windowInfo.lpszMenuName	= NULL;
	_windowInfo.lpszClassName	= desc.name.data();

	_windowInfo.lpfnWndProc = WndProc;

	_options				= desc.isChild ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW | WS_SYSMENU;
	_parentHandle			= desc.parentHandle;
	_windowsMode			= desc.windowMode;
	_rect					= desc.rect;
	if (_rect.x == 0 && _rect.y == 0)
	{
		auto longTtypeSize = _rect.size.Cast<LONG>();
		RECT rect = { 0, 0, longTtypeSize.w, longTtypeSize.h };
		AdjustWindowRect ( &rect, _options, FALSE );
		_rect.size.w = rect.right - rect.left;
		_rect.size.h = rect.bottom - rect.top;
		_rect.x = GetSystemMetrics (SM_CXSCREEN) / 2 - _rect.size.w / 2;
		_rect.y = GetSystemMetrics (SM_CYSCREEN) / 2 - _rect.size.h / 2;
	}
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
