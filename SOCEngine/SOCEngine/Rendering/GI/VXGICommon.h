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
			uint			packedSamplingCount;
			
			uint SetSamplingCount(uint diffuse, uint specular)
			{
				return ( packedSamplingCount = (diffuse & 0xffff) | (specular << 16) );
			}
		};
		struct VXGIDynamicInfo
		{
			Math::Vector3	startCenterWorldPos;
			uint			packedNumfOfLights;
		};
	}
}
