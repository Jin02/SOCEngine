#pragma once

#include "Vector3.h"
#include "ConstBuffer.h"
#include <algorithm>

#undef max

namespace Rendering
{
	struct MainRenderingSystemParam;

	namespace GI
	{
		struct VXGIStaticInfo
		{
			unsigned int	dimension	= 256;
			float			maxMipLevel = 8.0f;
			float			voxelSize	= 4.0f / 256.0f;

			union
			{
				uint packedSamplingCount	= (128 << 16) | 256;
				struct
				{
					ushort specularSamplingCount;
					ushort diffuseSamplingCount;
				};
			};
			
			VXGIStaticInfo() = default;
			VXGIStaticInfo(uint _dimension, float voxelizeSize)
				:	dimension(_dimension), voxelSize(voxelizeSize / float(dimension))
			{
				auto Log2 = [](float v) -> float
				{
					return log(v) / log(2.0f);
				};

				const uint mipmapGenOffset	= 2;
				const uint mipmapLevels		= std::max((uint)Log2((float)dimension) - mipmapGenOffset + 1, 1u);

				maxMipLevel = static_cast<float>(mipmapLevels);
			}
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
