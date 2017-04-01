//EMPTY_META_DATA

#include "GICommon.h"

RWByteAddressBuffer	OutVoxelAlbedoMap	: register( u0 );
RWByteAddressBuffer	OutVoxelNormalMap	: register( u1 );
RWByteAddressBuffer	OutVoxelEmissionMap : register( u2 );

[numthreads(8, 8, 8)]
void ClearVoxelMapCS(	uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	uint idx		= GetFlattedVoxelIndex(globalIdx, gi_dimension);
	uint address	= idx * 4;

	OutVoxelAlbedoMap.Store(address, 0);
	OutVoxelNormalMap.Store(address, 0);
	OutVoxelEmissionMap.Store(address, 0);
}