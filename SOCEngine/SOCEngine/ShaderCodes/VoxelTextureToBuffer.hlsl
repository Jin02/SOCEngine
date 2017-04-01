//EMPTY_META_DATA

#include "../GlobalDefine.h"

uint GetFlattedVoxelIndex(uint3 voxelIndex, uint dimension)
{
	uint sqDimension	= dimension * dimension;
	uint flat			= voxelIndex.x + (voxelIndex.y * dimension) + (voxelIndex.z * sqDimension);
	return flat;
}

uint GetFlattedVoxelIndexWithFaceIndex(uint3 voxelIndex, uint faceIndex, uint dimension)
{
	uint sqDimension		= dimension * dimension;

	uint fullLength			= (sqDimension * dimension);
	uint faceOffset			= fullLength * faceIndex;

	uint localFlattedIdx	= voxelIndex.x + (voxelIndex.y * dimension) + (voxelIndex.z * sqDimension);
	return faceOffset  +  localFlattedIdx;
}

#if defined(USE_DEBUG_TEXTURE_INJECTION)
RWTexture3D<uint>	InputVoxelTexture	: register( u0 );
#else
RWByteAddressBuffer	InputVoxelTexture	: register( u0 );
#endif

RWBuffer<uint>		OutputBuffer		: register( u1 );

cbuffer InfoCB : register( b0 )
{
	uint	dimension;
	uint3	dummy;
};

[numthreads(8, 8, 8)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	uint flatIdx = GetFlattedVoxelIndex(globalIdx, dimension);

#if defined(USE_FACE_INDEX)
	[unroll]
	for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
	{
		uint3 voxelIdx = globalIdx;
		voxelIdx.x += faceIdx * dimension;

		uint idx = voxelIdx.x + (voxelIdx.y * dimension * 6) + (voxelIdx.z * dimension * dimension * 6);

#if defined(USE_DEBUG_TEXTURE_INJECTION)
		OutputBuffer[idx].x = InputVoxelTexture[voxelIdx];
#else
		OutputBuffer[idx].x = InputVoxelTexture.Load(flatIdx * 4);
#endif

	}
#else
	uint idx = globalIdx.x + (globalIdx.y * dimension) + (globalIdx.z * dimension * dimension);

#if defined(USE_DEBUG_TEXTURE_INJECTION)
	OutputBuffer[idx].x = InputVoxelTexture[globalIdx];
#else
	OutputBuffer[idx].x = InputVoxelTexture.Load(flatIdx * 4);
#endif

#endif
}