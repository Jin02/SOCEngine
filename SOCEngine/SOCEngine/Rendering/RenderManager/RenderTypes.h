#pragma once

namespace Rendering
{
	enum class RenderType
	{
		GBuffer_AlphaBlend				= 0,
		GBuffer_Opaque					= 1,
		Forward_AlphaTestWithDiffuse	= 2,
		Forward_Transparency			= 3,
		Forward_DepthOnly				= 4,
		Voxelization					= 5,
		MAX_NUM
	};
}