#pragma once

namespace Rendering
{
	enum class RenderType : unsigned int
	{
		GBuffer_Opaque,
		GBuffer_AlphaBlend,
		Forward_Transparency,
		Forward_OnlyDepth,
		Forward_AlphaTestWithDiffuse,
		Forward_MomentDepth,
		Forward_MomentDepthWithAlphaTest,
		Voxelization,
		MAX_NUM
	};
}