#pragma once

namespace Rendering
{
	enum class RenderType : unsigned int
	{
		GBuffer_AlphaBlend					= 0,
		Forward_AlphaTestWithDiffuse		= 1,
		GBuffer_Opaque						= 2,
		Forward_Transparency				= 3,
		Forward_DepthOnly					= 4,
		Forward_MomentDepth					= 5,
		Voxelization						= 6,
		MAX_NUM
	};
}