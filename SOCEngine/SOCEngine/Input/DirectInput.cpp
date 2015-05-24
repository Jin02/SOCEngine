#include "DirectInput.h"

using namespace Device;

DirectInput::DirectInput()
	: _directInput(nullptr), _keyboardDevice(nullptr), _mouseDevice(nullptr)
{
	_keyboardState.fill(0);
	memset(&_mouseState, 0, sizeof(DIMOUSESTATE));
}

DirectInput::~DirectInput()
{
	SAFE_RELEASE_INPUT_DEVICE(_mouseDevice);
	SAFE_RELEASE_INPUT_DEVICE(_keyboardDevice);
	SAFE_RELEASE(_directInput);
}

bool DirectInput::Initialize(HINSTANCE hIst, HWND hWnd, const Math::Size<unsigned int>& screenSize)
{
	_screenSize = screenSize;

	HRESULT hr = DirectInput8Create(hIst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, nullptr);
	if( FAILED(hr) )
		return false;

	hr = _directInput->CreateDevice(GUID_SysKeyboard, &_keyboardDevice, nullptr);
	if( FAILED(hr) )
		return false;

	hr = _keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if( FAILED(hr) )
		return false;

	hr = _keyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if( FAILED(hr) )
		return false;

	hr = _keyboardDevice->Acquire();
	if( FAILED(hr) )
		return false;

	hr = _directInput->CreateDevice(GUID_SysMouse, &_mouseDevice, NULL);
	if( FAILED(hr) )
		return false;

	hr = _mouseDevice->SetDataFormat(&c_dfDIMouse);
	if( FAILED(hr) )
		return false;

	hr = _mouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if( FAILED(hr) )
		return false;

	hr = _mouseDevice->Acquire();
	if( FAILED(hr) )
		return false;

	return true;
}

void DirectInput::Update()
{
	HRESULT hr = -1;

	hr = _keyboardDevice->GetDeviceState(sizeof(_keyboardState), (void*)&_keyboardState);
	if(FAILED(hr))
	{
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			_keyboardDevice->Acquire();
	}

	hr = _mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&_mouseState);
	if(FAILED(hr))
	{
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			_mouseDevice->Acquire();
	}

	//Calc Mouse Pos
	{
		_mousePosition.x += _mouseState.lX;
		_mousePosition.y += _mouseState.lY;

		if(_mousePosition.x < 0)				_mousePosition.x = 0;
		if(_mousePosition.y < 0)				_mousePosition.y = 0;	
		if(_mousePosition.x > _screenSize.w)	_mousePosition.x = (float)_screenSize.w;
		if(_mousePosition.y > _screenSize.h)	_mousePosition.y = (float)_screenSize.h;
	}
}