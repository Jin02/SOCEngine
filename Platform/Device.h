#pragma once

#if defined(WIN32)
#include <Windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "Application.h"

#elif defined(__APPLE__)
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
#include <time.h>
#endif

#include "Graphics.h"