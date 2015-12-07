//EMPTY_META_DATA

#define VOXELIZATION

#include "Voxelization_Common.h"

[numthreads(8, 8, 8)]
void ClearVoxelMapCS(	uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	OutAnistropicVoxelAlbedoTexture[globalIdx]		= 0;
	OutAnistropicVoxelNormalTexture[globalIdx]		= 0;
	OutAnistropicVoxelEmissionTexture[globalIdx]	= 0;
}