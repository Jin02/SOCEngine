#pragma once

#include "Matrix.h"

namespace Rendering
{
	struct TransformCB
	{
		enum class ChangeState : uint
		{
			No,
			HasChanged,
			HadChanged,
			MAX
		};
		
		Math::Matrix world;
		Math::Matrix worldInvTranspose;

		Math::Matrix prevWorld;
	};
};
