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

		ReflectionProbe_Transparency,
		ReflectionProbe_OnlyDepth,
		ReflectionProbe_AlphaTestWithDiffuse,
		ReflectionProbe_OnlyFrontFace,

		Voxelization,
		MAX_NUM
	};
}