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
		UV0			= (1 << 0),
		UV1			= (1 << 1),
		NORMAL		= (1 << 2),
		TANGENT		= (1 << 3),
		BINORMAL	= (1 << 4),
		COLOR		= (1 << 5),
		BONE		= (1 << 6), //BONE ID WITH WEIGHT -> float2(id, weight).
//		BONE1		= (1 << 7), //BONE ID WITH WEIGHT -> float2(id, weight).
//		BONE2		= (1 << 8), //BONE ID WITH WEIGHT -> float2(id, weight).
//		BONE3		= (1 << 9), //BONE ID WITH WEIGHT -> float2(id, weight).
		USERS		= (1 << 15),
		MAX			= USERS
	};
	using BufferFlag = DefaultVertexInputTypeFlag;
}
