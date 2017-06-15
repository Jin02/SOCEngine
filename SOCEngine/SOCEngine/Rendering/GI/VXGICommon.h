#pragma once

#include "Vector3.h"
#include "ConstBuffer.h"

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
			Math::Vector3	startCenterWorldPos	= Math::Vector3(0.0f, 0.0f, 0.0f);
			uint			packedNumfOfLights	= 0;
		};

		using VXGIStaticInfoCB	= Buffer::ExplicitConstBuffer<VXGIStaticInfo>;
		using VXGIDynamicInfoCB	= Buffer::ExplicitConstBuffer<VXGIDynamicInfo>;

		struct VXGIInfoCB
		{
			VXGIStaticInfoCB		staticInfoCB;
			VXGIDynamicInfoCB		dynamicInfoCB;
		};
	}
}
