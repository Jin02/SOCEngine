#pragma once

namespace Rendering
{
	enum class RenderType : unsigned int
	{
		GBuffer_Opaque					,
		GBuffer_AlphaBlend				,
		Forward_Transparency			,
		Forward_OnlyDepth				,
		Forward_AlphaTestWithDiffuse	,
		Forward_MomentDepth				,
		Voxelization					,
		MAX_NUM
	};
}