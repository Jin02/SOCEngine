#pragma once

namespace Rendering
{
	enum class RenderType
	{
		GBuffer_Opaque					= 0,
		GBuffer_AlphaBlend				= 1,
		Forward_Transparency			= 2,
		Forward_DepthOnly				= 3,
		Forward_AlphaTestWithDiffuse	= 4,
		Voxelization					= 5,
		MAX_NUM
	};
}