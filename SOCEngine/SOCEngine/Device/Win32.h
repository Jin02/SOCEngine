#pragma once

#include <Windows.h>

#include "Common.h"
#include "Rect.h"

namespace Device
{
	class Win32
	{
	private:
		Math::Rect<unsigned int>	_rect;

		WNDCLASSEX					_windowInfo;
		HWND						_parentHandle;
		HWND						_handle;
		unsigned int				_options;
		bool						_windowsMode;

	private:
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		Win32(Math::Rect<unsigned int> &rect, HINSTANCE Instance, const char* name, bool windowMode, bool isChild, HWND parentHandle = NULL);
		~Win32(void);

	public:
		bool Initialize();
		void Destroy();

	public:
		bool IsChild();
		GET_ACCESSOR(IsWindowMode, bool, _windowsMode);
		GET_ACCESSOR(Handle, HWND, _handle);

	public:
		GET_ACCESSOR(Rect, const Math::Rect<unsigned int>&, _rect);
		GET_ACCESSOR(Size, const Math::Size<unsigned int>&, _rect.size);
		
	protected:
		SET_ACCESSOR(Rect, const Math::Rect<unsigned int>&, _rect);
	};
}