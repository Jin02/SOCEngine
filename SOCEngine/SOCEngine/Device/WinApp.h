#pragma once

#include <Windows.h>
#include <string>

#include "Common.h"
#include "Rect.h"

namespace Core
{
	class Launcher;
}

namespace Device
{
	class WinApp
	{
	public:
		struct Desc
		{
			Rect<uint>		rect			= Rect<uint>(0, 0, 800, 600);
			HINSTANCE		instance		= NULL;
			std::string		name			= "SOCEngine";
			bool			windowMode		= false;
			bool			isChild			= false;
			HWND			parentHandle	= NULL;

			Desc() = default;
		};

	public:
		WinApp(const Desc& desc);

		GET_CONST_ACCESSOR(IsChild,			bool,		_options == (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN));
		GET_CONST_ACCESSOR(IsWindowMode,	bool,		_windowsMode);
		GET_CONST_ACCESSOR(Handle,			HWND,		_handle);

	private:
		friend class Core::Launcher;
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void Initialize();
		void Destroy();

	private:
		Rect<uint>	_rect;
		WNDCLASSEX	_windowInfo;
		HWND		_parentHandle;
		HWND		_handle;
		uint		_options;
		bool		_windowsMode;
	};
}
