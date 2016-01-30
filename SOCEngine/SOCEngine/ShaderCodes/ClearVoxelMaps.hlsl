//EMPTY_META_DATA

#define USE_OUT_ANISOTROPIC_VOXEL_TEXTURES

#include "Voxelization_Common.h"

[numthreads(8, 8, 8)]
void ClearVoxelMapCS(	uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	OutVoxelAlbedoTexture[globalIdx]		= 0;
	OutVoxelNormalTexture[globalIdx]		= 0;
	OutVoxelEmissionTexture[globalIdx]	= 0;
}