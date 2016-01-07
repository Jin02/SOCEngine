#include "Director.h"
#include "ResourceManager.h"

using namespace Device;
using namespace Resource;

Director::Director(void) :
	_elapse(0.0f), _fps(0), _win(nullptr), _directX(nullptr),
	_scene(nullptr), _nextScene(nullptr), _exit(false)
{
}

Director::~Director(void)
{
}

void Director::CalculateElapse()
{
	static BaseTimer	staticTimer;
	static float before = 0.0f;
	float start;

	staticTimer.CheckElapsed(start);
	_elapse = start - before;
	_elapse = max((float)0, _elapse);

	before = start;
}

void Director::CalculateFPS()
{
	static int frameCnt = 0;
	static float elapsed = 0;

	frameCnt++;
	elapsed += _elapse;

	if( elapsed != 0.0f )
	{
		_fps = (float)frameCnt / elapsed;
		elapsed = 0;
		frameCnt = 0;
	}
}

void Director::Initialize(const Math::Rect<unsigned int>& windowRect, const Math::Rect<unsigned int>& renderScreenRect, HINSTANCE instance, const char* name, bool windowMode, bool isChild, HWND parentHandle)
{
	_win = new Win32(windowRect, instance, name, windowMode, isChild, parentHandle);
	ASSERT_COND_MSG(_win->Initialize(), "Error, can not create windows");

	_directX = new DirectX();
	ASSERT_COND_MSG(_directX->InitDevice(_win, renderScreenRect), "Error, can not create directX device");

	ResourceManager::SharedInstance()->Initialize();
}

void Director::Run()
{
	Timer::GetDefaultTimer();
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	while( (msg.message != WM_QUIT) && (_exit == false) )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if(_scene)
			{
				CalculateElapse();
				CalculateFPS();
				Core::Scene::State state = _scene->GetState();

				if(state == Core::Scene::State::Loop)
				{
					_scene->Update(_elapse);
					_scene->RenderPreview();
					_scene->Render();
				}
				else if(state == Core::Scene::State::Init)
					_scene->Initialize();
				else if(state == Core::Scene::State::End)
				{
					_scene->Destroy();
					_scene = _nextScene;
				}
			}
			else continue;				
		}
	}

	Resource::ResourceManager* resourceMgr = Resource::ResourceManager::SharedInstance();
	resourceMgr->DestroyManagers();
	Resource::ResourceManager::Destroy();
}

void Director::Exit()
{
	if(_exit) return;
	_exit = true;

	_scene->StopState();

	if(_scene)
	{
		_scene->Destroy();
		delete _scene;
	}

	if(_nextScene)
	{
		_nextScene->Destroy();
		delete _nextScene;
	}

	SAFE_DELETE(_directX);
	SAFE_DELETE(_win);
}