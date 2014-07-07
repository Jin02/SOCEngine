#pragma once

#include "Device.h"
#include "SingleTon.h"
#include "Rect.h"
#include "Color.h"
#include "BaseScene.h"

namespace Device
{
	class DeviceDirector : public Utility::SingleTon<DeviceDirector>
	{
	private:
		float elapse;
		float  fps;	

	private:
		Graphics *graphics;
		Application *app;
		Core::BaseScene* scene;
		Core::BaseScene* nextScene;

	public:
		DeviceDirector(void);
		~DeviceDirector(void);

	private:
		void CalculateElapse();
		void CalculateFPS();

	public:
#if defined(WIN32) && !defined(_USE_GL_DEFINES)
		struct WindowsInitOption
		{
			Math::Rect<int> rect;
			HINSTANCE instance;
			const char* name;
			bool windowMode;
			bool isChild;
			HWND parentHandle;

			WindowsInitOption(HINSTANCE hInst);
			WindowsInitOption(Math::Rect<int> rect, HINSTANCE instance, const char* name, bool windowMode, bool isChild, HWND parentHandle);
		};
		bool Initialize(PresentInterval interval, WindowsInitOption &options);

#elif defined(__APPLE__) || defined(_USE_GL_DEFINES)

#endif

		void Run();
		void Destroy();

	public:
		Graphics* GetGraphics();
		Application* GetApplication();

		Math::Size<int>& GetSize();

		void SetScene(Core::BaseScene* scene);
		void SetNextScene(Core::BaseScene *scene);

		Core::BaseScene* GetScene();
	};
}