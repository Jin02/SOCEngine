#include "Launcher.h"
#include "DirectX.h"
#include "Engine.h"

using namespace Core;
using namespace Device;

void Launcher::Run(const WinApp::Desc& desc, const Rect<uint>& viewport, bool useMSAA, IScene* scene)
{
	{
		WinApp win(desc);
		DirectX dx(viewport.Cast<float>());
		Engine engine(dx);

		// Init System
		{
			win.Initialize();
			dx.Initialize(win, viewport, useMSAA);
			engine.Initialize(scene);
		}

		// Loop
		{
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));

			engine.StartLoop();
			while ((msg.message != WM_QUIT) & (engine.GetExit() == false))
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					engine.RunScene();
				}
			}
		}

		// Destroy
		{
			win.Destroy();
			engine.Destroy();
			dx.Destroy();
		}
	}

#ifdef DEBUG
	DirectX::ReportLiveObjects();
#endif
}