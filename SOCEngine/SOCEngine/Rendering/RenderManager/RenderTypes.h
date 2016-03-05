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
		Forward_ReflectionProbe,
		Forward_ReflectionProbeWithAlphaTest,
		Forward_ReflectionProbeTransparency,
		Voxelization,
		MAX_NUM
	};
}