#pragma once

#include "Vector3.h"

namespace Rendering
{
	namespace GI
	{
		struct GlboalStaticInfo
		{
			unsigned int	maxCascadeWithVoxelDimensionPow2;
			float			maxMipLevel;
			float			occlusion;					//dummy
			float			diffuseHalfConeMaxAngle;	//dummy
		};

		struct GlobalDynamicInfo
		{
			float			initVoxelSize;
			Math::Vector3	startCenterWorldPos;
		};
	}
}