#pragma once

#include "DirectX.h"
#include "Win32.h"
#include "Singleton.h"
#include "Timer.h"
#include "Scene.h"

namespace Device
{
	class Director : public Singleton<Director>
	{
	private:
		friend class Singleton<Director>;

	private:
		float				_elapse;
		float				_fps;
		Win32*				_win;
		DirectX*			_directX;

		Core::Scene*		_scene;
		Core::Scene*		_nextScene;

	protected:
		Director(void);
		~Director(void);

	private:
		void CalculateElapse();
		void CalculateFPS();

	public:
		void Initialize(Math::Rect<unsigned int> &rect, HINSTANCE instance, const char* name, bool windowMode, bool isChild, HWND parentHandle = NULL);
		void Run();

	public:
		GET_ACCESSOR(Win, const Win32*, _win);
		GET_ACCESSOR(DirectX, const DirectX*, _directX);
		GET_ACCESSOR(WindowSize, const Math::Size<unsigned int>&, _win->GetSize());

		GET_SET_ACCESSOR(CurrentScene, Core::Scene*, _scene);
		GET_SET_ACCESSOR(NextScene, Core::Scene*, _nextScene);
	};

}