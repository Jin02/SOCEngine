#ifdef WIN32

#include "Win32.h"
#include "Director.h"

namespace Device
{
	LRESULT Win32::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Director* director = Director::GetInstance();
		Win32* win = const_cast<Win32*>(director->GetWin());
		Core::Scene* scene = director->GetCurrentScene();

		if(WM_MOUSEMOVE <= msg && msg <= WM_MBUTTONDBLCLK)
		{
			Mouse::Type type = Mouse::Type::Unknown;
			switch(msg)
			{
			case WM_LBUTTONDOWN:
				win->_mouse.states[(uint)(type = Mouse::Type::Left)] = Mouse::Action::Down;
				break;
			case WM_LBUTTONUP:
				win->_mouse.states[(uint)(type = Mouse::Type::Left)] = Mouse::Action::Up;
				break;
			case WM_LBUTTONDBLCLK:
				win->_mouse.states[(uint)(type = Mouse::Type::Left)] = Mouse::Action::DoubleClick;
				break;

			case WM_RBUTTONDOWN:
				win->_mouse.states[(uint)(type = Mouse::Type::Right)] = Mouse::Action::Down;
				break;
			case WM_RBUTTONUP:
				win->_mouse.states[(uint)(type = Mouse::Type::Right)] = Mouse::Action::Up;
				break;
			case WM_RBUTTONDBLCLK:
				win->_mouse.states[(uint)(type = Mouse::Type::Right)] = Mouse::Action::DoubleClick;
				break;

			case WM_MBUTTONDOWN:
				win->_mouse.states[(uint)(type = Mouse::Type::Middle)] = Mouse::Action::Down;
				break;
			case WM_MBUTTONUP:
				win->_mouse.states[(uint)(type = Mouse::Type::Middle)] = Mouse::Action::Up;
				break;
			case WM_MBUTTONDBLCLK:
				win->_mouse.states[(uint)(type = Mouse::Type::Middle)] = Mouse::Action::DoubleClick;
				break;
			}

			scene->OnInput(win->_mouse, win->_keyboard);
			if(win->_mouse.states[(uint)type] != Mouse::Action::Down)
				win->_mouse.states[(uint)type] = Mouse::Action::None;
		}
		else if(WM_KEYDOWN == msg)
		{
			win->_keyboard.states[wParam] = Keyboard::Type::Down;
			scene->OnInput(win->_mouse, win->_keyboard);
		}
		else if(msg == WM_KEYUP)
		{
			win->_keyboard.states[wParam] = Keyboard::Type::Up;
			scene->OnInput(win->_mouse, win->_keyboard);
			win->_keyboard.states[wParam] = Keyboard::Type::None;
		}
		else if(msg == WM_DESTROY || msg == WM_CLOSE)
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

		memset(&_mouse, 0, sizeof(Mouse));
		memset(&_keyboard, 0, sizeof(Keyboard));
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