#pragma once

#include "Vector3.h"

namespace Rendering
{
	namespace GI
	{
		struct GlobalInfo
		{
			unsigned int	maxCascadeWithVoxelDimensionPow2;
			float			initVoxelSize;
			float			initWorldSize;
			float			maxMipLevel;
		};
	}
}