#pragma once

#include "Matrix.h"

namespace Rendering
{
	struct TransformCB
	{
		Math::Matrix world;
		Math::Matrix worldInvTranspose;

		Math::Matrix prevWorld;
	};
};
