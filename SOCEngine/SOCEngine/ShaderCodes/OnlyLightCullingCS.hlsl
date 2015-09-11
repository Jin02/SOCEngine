//EMPTY_META_DATA

RWBuffer<uint> g_outPerLightIndicesInTile : register(u0);

#define USE_ATOMIC

#if defined(USE_PARALLEL)
#include "LightCullingCompareParallelCS.h"

[numthreads(TILE_RES_HALF, TILE_RES_HALF, 1)]

#elif defined(USE_ATOMIC)
#include "LightCullingCompareAtomicCS.h"

[numthreads(TILE_RES, TILE_RES, 1)]
#endif

void OnlyLightCullingCS(uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	uint pointLightCountInThisTile = 0;
	LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile);

#if defined(USE_PARALLEL)
	uint idxInTile	= localIdx.x + localIdx.y * TILE_RES_HALF;
#elif defined(USE_ATOMIC)
	uint idxInTile	= localIdx.x + localIdx.y * TILE_RES;
#endif
	uint tileIdx	= groupIdx.x + groupIdx.y * GetNumTilesX();
	uint startOffset = tbrParam_maxNumOfPerLightInTile * tileIdx + 1;

	if(idxInTile == 0)
		g_outPerLightIndicesInTile[startOffset - 1] = (((s_lightIndexCounter - pointLightCountInThisTile) & 0x0000ffff) << 16) | (pointLightCountInThisTile & 0x0000ffff);

	for(uint i=idxInTile; i<pointLightCountInThisTile; i+=THREAD_COUNT)
		g_outPerLightIndicesInTile[startOffset + i] = s_lightIdx[i];

	for(uint j=(idxInTile + pointLightCountInThisTile); j<s_lightIndexCounter; j+=THREAD_COUNT)
		g_outPerLightIndicesInTile[startOffset + j] = s_lightIdx[j];
}