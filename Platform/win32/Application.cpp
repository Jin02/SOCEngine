#ifdef WIN32

#include "Application.h"
#include <cassert>

namespace Device
{

		LRESULT Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if(msg == WM_DESTROY || msg == WM_CLOSE)
			{
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProc( hWnd, msg, wParam, lParam );
		}

		Application::Application(Math::Rect<int> &rect, HINSTANCE Instance, const char* name, bool windowMode, bool isChild, HWND parentHandle)
		{
			windowInfo.cbSize			= sizeof(WNDCLASSEX);
			windowInfo.style			= CS_CLASSDC;
			windowInfo.hInstance		= Instance;//GetModuleHandle(NULL);
			windowInfo.cbClsExtra		= 0L;
			windowInfo.cbWndExtra		= 0L;
			windowInfo.hIcon			= NULL;
			windowInfo.hCursor			= NULL;
			windowInfo.hbrBackground	= NULL;
			windowInfo.hIconSm			= NULL;
			windowInfo.lpszMenuName		= NULL;
			windowInfo.lpszClassName	= name;

			windowInfo.lpfnWndProc		= WndProc;
			options = isChild ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW | WS_SYSMENU;

			SetRect(rect);
			this->name = name;
			this->parentHandle = parentHandle;
			this->windowsMode = windowMode;
		}

		Application::~Application(void)
		{
			Destroy();
		}

		bool Application::Initialize()
		{
			RegisterClassEx(&windowInfo);

			handle = CreateWindow(name, name, options, 
				rect.x, rect.y, 
				rect.size.w, rect.size.h, 
				parentHandle, NULL, windowInfo.hInstance, NULL);

			assert(handle);

			if( options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN) )
				ShowWindow(handle, SW_SHOWDEFAULT);

			return true;
		}
		void Application::Destroy()
		{
			UnregisterClass( name, windowInfo.hInstance );

			if( options != (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN) )
				DestroyWindow(handle);
			else PostQuitMessage(0);
		}

		bool Application::IsChild()
		{
			return options == (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
		}
		bool Application::IsWindowMode()
		{
			return windowsMode;
		}
		HWND Application::GetHandle()
		{
			return handle;
		}

		Math::Rect<int> Application::GetRect()
		{
			return rect;
		}

		Math::Size<int> Application::GetSize()
		{
			return rect.size;
		}

		void Application::SetRect(Math::Rect<int> r)
		{
			rect = r;
		}
	
}

#endif