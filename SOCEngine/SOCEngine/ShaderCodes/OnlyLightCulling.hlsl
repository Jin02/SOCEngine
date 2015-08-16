//EMPTY_META_DATA

#include "LightCulling_CS.h"

RWBuffer<uint> g_outPerLightIndicesInTile	: register( u0 );

[numthreads(TILE_RES_HALF, TILE_RES_HALF, 1)]
void OnlyLightCullingCS(uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	float minZ, maxZ;
	uint pointLightCountInThisTile = 0;

	LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile, minZ, maxZ);

	uint idxInTile	= localIdx.x + localIdx.y * TILE_RES_HALF;
	uint tileIdx	= groupIdx.x + groupIdx.y * GetNumTilesX();

	uint startOffset = g_maxNumOfperLightInTile * tileIdx + 1;

	if(idxInTile == 0)
		g_outPerLightIndicesInTile[startOffset - 1] = (((s_lightIndexCounter - pointLightCountInThisTile) & 0x0000ffff) << 16) | (pointLightCountInThisTile & 0x0000ffff);

	for(uint i=idxInTile; i<pointLightCountInThisTile; i+=LIGHT_CULLING_THREAD_COUNT)
		g_outPerLightIndicesInTile[startOffset + i] = s_lightIdx[i];

	for(uint j=(idxInTile + pointLightCountInThisTile); j<s_lightIndexCounter; j+=LIGHT_CULLING_THREAD_COUNT)
		g_outPerLightIndicesInTile[startOffset + j] = s_lightIdx[j];
}