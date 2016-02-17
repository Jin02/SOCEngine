#pragma once

#include "DirectX.h"
#include "Win32.h"
#include "Singleton.h"
#include "Timer.h"
#include "Scene.h"
#include "GlobalDefine.h"

namespace Device
{
	class Director : public Singleton<Director>
	{
	private:
		friend class Singleton<Director>;

	private:
		float					_elapse;
		float					_fps;
		Win32*					_win;
		DirectX*				_directX;
		Core::Scene*			_scene;
		Core::Scene*			_nextScene;
		bool					_exit;

	protected:
		Director(void);
		virtual ~Director(void);

	private:
		void CalculateElapse();
		void CalculateFPS();

	public:
		void Initialize(const Math::Rect<unsigned int>& windowRect, const Math::Rect<unsigned int>& renderScreenRect, HINSTANCE instance, const char* name, bool windowMode, bool isChild, HWND parentHandle = NULL);
		void Run();
		void Exit();

	public:
		GET_ACCESSOR(Win, const Win32*, _win);
		GET_ACCESSOR(DirectX, const DirectX*, _directX);
		GET_ACCESSOR(WindowSize, const Math::Size<unsigned int>&, _win->GetSize());
		GET_ACCESSOR(BackBufferSize, const Math::Size<unsigned int>&, _directX->GetBackBufferSize());

		GET_SET_ACCESSOR(CurrentScene, Core::Scene*, _scene);
		GET_SET_ACCESSOR(NextScene, Core::Scene*, _nextScene);
	};
}