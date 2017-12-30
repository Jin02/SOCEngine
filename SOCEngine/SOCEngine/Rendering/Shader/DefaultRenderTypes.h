#pragma once

namespace Rendering
{
	enum class DefaultRenderType : unsigned int
	{
		GBuffer_Opaque							,
		GBuffer_AlphaTest						,
		Forward_Transparency					,
		Forward_OnlyDepth						,
		Forward_AlphaTestWithDiffuse			,
		Forward_PCSSViewDepth					,
		Forward_PCSSViewDepthAlphaTest			,
		Voxelization							,
		MAX_NUM
	};

	enum class DefaultVertexInputTypeFlag : unsigned int
	{
		UV0		= 1,
		UV1		= 2,
		NORMAL	= 4,
		TANGENT = 8,
		COLOR	= 16,
		BONE	= 32, //BONE ID WITH WEIGHT -> float2(id, weight).
		USERS	= 64,
		MAX		= USERS
	};
	using BufferFlag = DefaultVertexInputTypeFlag;
}
