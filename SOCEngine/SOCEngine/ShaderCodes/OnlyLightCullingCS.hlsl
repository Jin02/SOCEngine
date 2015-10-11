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
#if defined(USE_PARALLEL)
	uint localTileIdx	= localIdx.x + localIdx.y * TILE_RES_HALF;
#elif defined(USE_ATOMIC)
	uint localTileIdx	= localIdx.x + localIdx.y * TILE_RES;
#endif

	if(localTileIdx == 0)
	{
		s_lightIndexCounter	= 0;
		s_minZ = 0x7f7fffff; //float max as uint
		s_maxZ = 0;
	}

	uint pointLightCountInThisTile = 0;
	LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile);
	GroupMemoryBarrierWithGroupSync();

	uint globalTileIdx = groupIdx.x + groupIdx.y * GetNumTilesX();
	uint startOffset = tbrParam_maxNumOfPerLightInTile * globalTileIdx + 1;

	if(localTileIdx == 0)
	{
		uint spotLightCount		= s_lightIndexCounter - pointLightCountInThisTile;
		uint pointLightCount	= pointLightCountInThisTile & 0x0000ffff;
		g_outPerLightIndicesInTile[startOffset - 1] = (spotLightCount << 16) | pointLightCount;
	}

	for(uint i=localTileIdx; i<pointLightCountInThisTile; i+=THREAD_COUNT)
		g_outPerLightIndicesInTile[startOffset + i] = s_lightIdx[i];

	for(uint j=(localTileIdx + pointLightCountInThisTile); j<s_lightIndexCounter; j+=THREAD_COUNT)
		g_outPerLightIndicesInTile[startOffset + j] = s_lightIdx[j];
}