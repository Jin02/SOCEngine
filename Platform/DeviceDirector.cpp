#include "DeviceDirector.h"
#include "Timer.h"
#include <cassert>

namespace Device
{
#if defined(WIN32) && !defined(_USE_GL_DEFINES)
	DeviceDirector::WindowsInitOption::WindowsInitOption(HINSTANCE hInst)
	{
		rect = Math::Rect<int>(0, 0, 800, 800);
		instance = hInst;
		name = "TEST";
		windowMode = true;
		isChild = false;
		parentHandle = NULL;
	}

	DeviceDirector::WindowsInitOption::WindowsInitOption(Math::Rect<int> rect, HINSTANCE instance, const char* name, bool windowMode, bool isChild, HWND parentHandle)
	{
		this->rect = rect;
		this->instance = instance;
		this->name = name;
		this->windowMode = windowMode;
		this->isChild = isChild;
		this->parentHandle = parentHandle;
	}
#elif defined(__APPLE__) || defined(_USE_GL_DEFINES)

#endif

	DeviceDirector::DeviceDirector(void) : graphics(NULL), app(NULL)
	{
		scene = nullptr;
		nextScene = nullptr;

		fps = 0.0f;
		elapse = 0.0f;
	}

	DeviceDirector::~DeviceDirector(void)
	{
		Destroy();
	}

	void DeviceDirector::Destroy()
	{
		delete graphics;
		graphics = NULL;

		delete app;
		app = NULL;
	}

	void DeviceDirector::CalculateElapse()
	{
		static Core::BaseTimer	staticTimer;
		static float before = 0.0f;
		float start;

		staticTimer.CheckElapsed(start);
		elapse = start - before;
		elapse = Utility::Max((float)0, elapse);

		before = start;
	}

	void DeviceDirector::CalculateFPS()
	{
		static int frameCnt = 0;
		static float elapsed = 0;

		frameCnt++;
		elapsed += elapse;

		if( elapsed >= 1.f )
		{
			fps = (float)frameCnt / elapsed;
			elapsed = 0;
			frameCnt = 0;
		}
	}

	Graphics* DeviceDirector::GetGraphics()
	{
		return graphics;
	}

	Application* DeviceDirector::GetApplication()
	{
		return app;
	}

	Math::Size<int>& DeviceDirector::GetSize()
	{
		return app->GetSize();
	}

	void DeviceDirector::SetScene(Core::BaseScene* scene)
	{
		this->scene = scene;
	}

	void DeviceDirector::SetNextScene(Core::BaseScene *scene)
	{
		nextScene  = scene;
	}

	Core::BaseScene* DeviceDirector::GetScene()
	{
		return scene;
	}

#if defined(WIN32) && !defined(_USE_GL_DEFINES)

	bool DeviceDirector::Initialize(PresentInterval interval, WindowsInitOption &options)
	{
		app = new Application(options.rect, options.instance, options.name, options.windowMode, options.isChild, options.parentHandle);
		assert(app->Initialize());

		graphics = new Graphics(interval, app);
		assert(graphics->Initialize());

		return true;
	}

	void DeviceDirector::Run()
	{
		Core::Timer::GetDefaultTimer();
		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );

		while( msg.message != WM_QUIT )
		{
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				if(scene)
				{
					CalculateElapse();
					CalculateFPS();
					Core::BaseScene::STATE state = scene->GetState();

					if(state == Core::BaseScene::STATE_LOOP)
					{
						scene->Update(elapse);
						scene->Render();
					}
					else if(state == Core::BaseScene::STATE_INIT)
						scene->Initialize();
					else
					{
						scene->Destroy();
						scene = nextScene;
					}
				}
				else continue;				
			}
		}
	}

#elif defined(__APPLE__) || defined(_USE_GL_DEFINES)

#endif

}