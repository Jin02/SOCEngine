//EMPTY_META_DATA

#include "ShaderCommon.h"

[numthreads(VOXEL_CONE_TRACING_TILE_RES, VOXEL_CONE_TRACING_TILE_RES, 1)]
void TileBasedDeferredShadingCS(uint3 globalIdx : SV_DispatchThreadID, 
								uint3 localIdx	: SV_GroupThreadID,
								uint3 groupIdx	: SV_GroupID)
{

}