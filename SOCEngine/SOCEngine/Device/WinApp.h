#pragma once

#include <Windows.h>
#include <string>

#include "Common.h"
#include "Rect.h"

namespace Device
{
	class WinApp
	{
	private:
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		WinApp(const Rect<uint> &rect, HINSTANCE Instance, const std::string& name, bool windowMode, bool isChild, HWND parentHandle = NULL);
		~WinApp(void);

	public:
		bool Initialize();
		void Destroy();

	public:
		GET_ACCESSOR(IsChild,		bool,		_options == (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN));
		GET_ACCESSOR(IsWindowMode,	bool,		_windowsMode);
		GET_ACCESSOR(Handle,		HWND,		_handle);

	private:
		Rect<uint>	_rect;
		WNDCLASSEX	_windowInfo;
		HWND		_parentHandle;
		HWND		_handle;
		uint		_options;
		bool		_windowsMode;
	};
}