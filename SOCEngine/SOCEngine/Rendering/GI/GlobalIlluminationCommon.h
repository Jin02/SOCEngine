#pragma once

#include "Vector3.h"

namespace Rendering
{
	namespace GI
	{
		struct GlobalInfo
		{
			union
			{
				struct
				{
					unsigned short maxNumOfCascade;
					unsigned short voxelDimensionPow2;
				};

				unsigned int	maxCascadeWithVoxelDimensionPow2;
			};

			float			initVoxelSize;
			float			initWorldSize;
			float			maxMipLevel;
		};
	}
}