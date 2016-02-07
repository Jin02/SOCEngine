//EMPTY_META_DATA

RWTexture3D<uint>	InputVoxelTexture	: register( u0 );
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

#if defined(USE_FACE_INDEX)
	[unroll]
	for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
	{
		uint3 voxelIdx = globalIdx;
		voxelIdx.x += faceIdx * dimension;

		uint idx = voxelIdx.x + (voxelIdx.y * dimension * 6) + (voxelIdx.z * dimension * dimension * 6);
		OutputBuffer[idx].x = InputVoxelTexture[voxelIdx];
	}
#else
	uint idx = globalIdx.x + (globalIdx.y * dimension) + (globalIdx.z * dimension * dimension);
	OutputBuffer[idx].x = InputVoxelTexture[globalIdx];
#endif
}