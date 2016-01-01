#pragma once

namespace Rendering
{
	enum class RenderType
	{
		GBuffer_Opaque					= 0,
		Forward_Transparency			= 1,
		GBuffer_AlphaBlend				= 2,
		Forward_AlphaTestWithDiffuse	= 3,
		Voxelization					= 4,
		Forward_DepthOnly				= 5,
		MAX_NUM
	};
}