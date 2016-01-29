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

	[unroll]
	for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
	{
		globalIdx.x += faceIdx;

		uint idx = globalIdx.x + (globalIdx.y * dimension) + (globalIdx.z * dimension * dimension);
		OutputBuffer[idx].x = InputVoxelTexture[globalIdx];
	}
}