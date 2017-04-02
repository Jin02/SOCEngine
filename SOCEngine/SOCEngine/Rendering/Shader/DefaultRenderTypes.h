#pragma once

namespace Rendering
{
	enum class DefaultRenderType : unsigned int
	{
		GBuffer_Opaque,
		GBuffer_AlphaBlend,

		Forward_Transparency,
		Forward_OnlyDepth,
		Forward_AlphaTestWithDiffuse,

		ReflectionProbe_Transparency,
		ReflectionProbe_OnlyDepth,
		ReflectionProbe_AlphaTestWithDiffuse,
		ReflectionProbe_OnlyFrontFace,

		Voxelization,
		MAX_NUM
	};

	enum class DefaultVertexInputTypeFlag : unsigned int
	{
		UV0	= 0b00000000000001,
		UV1	= 0b00000000000010,
		NORMAL	= 0b00000000000100,
		TANGENT = 0b00000000001000,
		COLOR	= 0b00000000010000,
		BONE	= 0b00000000100000, //BONE ID WITH WEIGHT -> float2(id, weight).
		USERS	= 0b10000000000000
	};
}
