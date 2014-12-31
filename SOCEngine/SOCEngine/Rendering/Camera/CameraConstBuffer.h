#pragma once

#include "EngineMath.h"

namespace Rendering
{
	namespace Camera
	{
		struct CameraConstBuffer
		{
			Math::Vector4 viewPos;
			float clippingNear, clippingFar;
			Math::Size<float> screenSize;
		};
	}
}