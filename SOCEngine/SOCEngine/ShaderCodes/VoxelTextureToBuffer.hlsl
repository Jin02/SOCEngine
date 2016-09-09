//EMPTY_META_DATA

#include "VoxelRawBufferCommon.h"

#define RAW_BUFFER

#ifndef RAW_BUFFER
RWTexture3D<uint>	InputVoxelTexture	: register( u0 );
#else
RWByteAddressBuffer	InputVoxelTexture	: register( u0 );
#endif

RWBuffer<uint>		OutputBuffer		: register( u1 );

cbuffer InfoCB : register( b0 )
{
	uint	curCascade;
	uint	dimension;
	uint2	dummy;
};

[numthreads(8, 8, 8)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	globalIdx.y += curCascade;

	uint flatIdx = GetFlattedVoxelIndex(globalIdx, curCascade, dimension);

#if defined(USE_FACE_INDEX)
	[unroll]
	for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
	{
		uint3 voxelIdx = globalIdx;
		voxelIdx.x += faceIdx * dimension;

		uint idx = voxelIdx.x + (voxelIdx.y * dimension * 6) + (voxelIdx.z * dimension * dimension * 6);

#ifndef RAW_BUFFER
		OutputBuffer[idx].x = InputVoxelTexture[voxelIdx];
#else
		OutputBuffer[idx].x = InputVoxelTexture.Load(flatIdx);
#endif

	}
#else
	uint idx = globalIdx.x + (globalIdx.y * dimension) + (globalIdx.z * dimension * dimension);

#ifndef RAW_BUFFER
	OutputBuffer[idx].x = InputVoxelTexture[globalIdx];
#else
	OutputBuffer[idx].x = InputVoxelTexture.Load(flatIdx);
#endif

#endif
}