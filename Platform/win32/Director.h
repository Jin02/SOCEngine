#pragma once

#include "DX.h"
#include "Win32.h"
#include "Singleton.h"

namespace Device
{

	class Director : public Singleton<Director>
	{
	private:
		friend class Singleton<Director>;

	private:
		float _elapse;
		float _fps;
		Win32*	_win;
		DX*		_directX;

	protected:
		Director(void)
		{
		}

		~Director(void)
		{
		}

	public:
		GET_ACCESSOR(Win, const Win32*, _win);
		GET_ACCESSOR(DirectX, const DX*, _directX);
		GET_ACCESSOR(WindowSize, const Math::Size<int>&, _win->GetSize());
	};

}