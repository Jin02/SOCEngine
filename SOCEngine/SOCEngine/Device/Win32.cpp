#ifdef WIN32

#include "Win32.h"

namespace Device
{
	LRESULT Win32::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if(msg == WM_DESTROY || msg == WM_CLOSE)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	Win32::Win32(Math::Rect<unsigned int> &rect, HINSTANCE Instance, const char* name, bool windowMode, bool isChild, HWND parentHandle)
	{
		//_name = name;

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
		_windowInfo.lpszClassName	= name;

		_windowInfo.lpfnWndProc		= WndProc;
		_options = isChild ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW | WS_SYSMENU;

		SetRect(rect);

		_parentHandle = parentHandle;
		_windowsMode = windowMode;

	}

	Win32::~Win32(void)
	{
		Destroy();
	}

	bool Win32::Initialize()
	{
		RegisterClassEx(&_windowInfo);

		_handle = CreateWindow(_windowInfo.lpszClassName, _windowInfo.lpszClassName, _options, 
			_rect.x, _rect.y, 
			_rect.size.w, _rect.size.h, 
			_parentHandle, NULL, _windowInfo.hInstance, NULL);

		//assert(_handle);

		if( _options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN) )
			ShowWindow(_handle, SW_SHOWDEFAULT);

		return true;
	}
	void Win32::Destroy()
	{
		UnregisterClass( _windowInfo.lpszClassName, _windowInfo.hInstance );

		if( _options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN) )
			DestroyWindow(_handle);
		else PostQuitMessage(0);
	}

	bool Win32::IsChild()
	{
		return _options == (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	}
}

#endif