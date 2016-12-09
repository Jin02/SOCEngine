#pragma once

#include "Vector3.h"

namespace Rendering
{
	namespace GI
	{
		struct VXGIStaticInfo
		{
			unsigned int	dimension;
			float			maxMipLevel;
			float			voxelSize;
			
			union
			{
				uint packedSamplingCount;
				struct
				{
					ushort specularSamplingCount;
					ushort diffuseSamplingCount;
				};
			};
			
		};
		struct VXGIDynamicInfo
		{
			Math::Vector3	startCenterWorldPos;
			uint			packedNumfOfLights;
		};
	}
}
