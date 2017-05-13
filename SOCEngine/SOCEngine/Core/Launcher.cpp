#include "Launcher.h"
#include "DirectX.h"
#include "Engine.h"

using namespace Core;
using namespace Device;

void Launcher::Run(const WinApp::Desc& desc, const Rect<uint>& viewport, bool useMSAA)
{
	WinApp win(desc);
	DirectX dx;
	Engine engine(dx);

	// Init System
	{
		win.Initialize();
		dx.Initialize(win, viewport, useMSAA);
		engine.Initialize();
	}

	// Loop
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		while ((msg.message != WM_QUIT) && (_exit == false))
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) == false)
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
	}
}