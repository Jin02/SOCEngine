#pragma once

#include "ShadowBufferForm.hpp"
#include "SpotLightShadow.h"

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			class SpotLightShadowBuffer final : public ShadowBufferForm<SpotLightShadow> {};
		}
	}
}