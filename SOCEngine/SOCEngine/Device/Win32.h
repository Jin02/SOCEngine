#pragma once

#include <Windows.h>

#include "Common.h"
#include "Rect.h"
#include <array>

#define MOUSE_LEFT		0
#define MOUSE_RIHGT		1

#define KEYBOARD	Device::Win32::Keyboard
#define MOUSE		Device::Win32::Mouse

namespace Device
{
	class Win32
	{
	public:
		struct Keyboard
		{
			enum class Type	{ None,	Down, Up };
			std::array<Type, 256> states;
		};

		struct Mouse
		{
			enum class Action				{ None, Down, DoubleClick, Up };
			enum class Type : unsigned int	{ Left, Middle, Right, Unknown };
			std::array<Action, 4> states;
			GET_ACCESSOR(Left, Action, states[(uint)Type::Left]);
			GET_ACCESSOR(Right, Action, states[(uint)Type::Right]);
			GET_ACCESSOR(Middle, Action, states[(uint)Type::Middle]);
		};


	private:
		Math::Rect<unsigned int>	_rect;

		WNDCLASSEX					_windowInfo;
		HWND						_parentHandle;
		HWND						_handle;
		unsigned int				_options;
		bool						_windowsMode;

		Keyboard					_keyboard;
		Mouse						_mouse;

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