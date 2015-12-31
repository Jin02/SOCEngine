#pragma once

namespace Rendering
{
	enum class RenderType
	{
		GBuffer_AlphaBlend,
		Forward_AlphaTest,
		GBuffer_Opaque,
		Forward_Transparency,
		Forward_DepthOnly,
		Voxelization,
		MAX_NUM
	};
}