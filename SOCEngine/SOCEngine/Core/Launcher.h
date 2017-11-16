#pragma once

#include "WinApp.h"
#include "IScene.h"

namespace Core
{
	class Launcher
	{
	public:
		static void Run(const Device::WinApp::Desc& desc,
						const Rect<uint>& viewport,
						bool useMSAA, IScene* scene);
	};
}