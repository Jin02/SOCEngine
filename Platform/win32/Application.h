#pragma once

#include <Windows.h>
#include "Rect.h"

namespace Device
{
	class Application
	{
	private:
		Math::Rect<int> rect;

		WNDCLASSEX		windowInfo;
		const char*	name;
		HWND			parentHandle;
		HWND			handle;
		unsigned int	options;
		bool			windowsMode;

	private:
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		Application(Math::Rect<int> &rect, HINSTANCE Instance, const char* name, bool windowMode, bool isChild, HWND parentHandle = NULL);
		~Application(void);

	public:
		bool Initialize();
		void Destroy();

	public:
		bool IsChild();
		bool IsWindowMode();
		HWND GetHandle();

	public:
		Math::Rect<int> GetRect();
		Math::Size<int> GetSize();

	protected:
		void SetRect(Math::Rect<int> r);
	};

}