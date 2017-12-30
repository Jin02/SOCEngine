#pragma once

#include "ShadowBufferForm.hpp"
#include "PointLightShadow.h"

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			class PointLightShadowBuffer final : public ShadowBufferForm<PointLightShadow> {};
		}
	}
}