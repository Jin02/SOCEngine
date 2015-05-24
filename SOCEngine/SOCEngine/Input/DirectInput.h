#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Size.h"
#include "Vector2.h"
#include "Common.h"
#include <array>

#define SAFE_RELEASE_INPUT_DEVICE(d) if(d){d->Unacquire(); d->Release(); d = 0;}

namespace Device
{
	class DirectInput
	{
	private:
		IDirectInput8*					_directInput;
		IDirectInputDevice8*			_keyboardDevice;
		IDirectInputDevice8*			_mouseDevice;

		std::array<unsigned char, 256>	_keyboardState;
		DIMOUSESTATE					_mouseState;

		Math::Vector2					_mousePosition;
		Math::Size<uint>				_screenSize;

	public:
		DirectInput();
		~DirectInput();

	public:
		bool Initialize(HINSTANCE hIst, HWND hWnd, const Math::Size<unsigned int>& screenSize);
		void Update();

	public:
		GET_ACCESSOR(MousePosition, const Math::Vector2&, _mousePosition);

		//GET_ACCESSOR(KeyboardState, const std::array<unsigned char, 256>&, _keyboardState);
		inline const std::array<unsigned char, 256>& GetKeyboardState() const { return _keyboardState; }
	};
}