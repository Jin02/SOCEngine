#pragma once

#include "WinApp.h"

namespace Core
{
	class Launcher
	{
	public:
		static void Run(const Device::WinApp::Param& winParam, const Rect<uint>& viewport, bool useMSAA);
	};
}