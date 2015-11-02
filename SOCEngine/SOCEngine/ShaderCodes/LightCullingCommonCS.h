//NOT_CREATE_META_DATA

#ifndef __SOC_LIGHT_CULLING_CS_COMMON_H__
#define __SOC_LIGHT_CULLING_CS_COMMON_H__

#include "ShaderCommon.h"

#define EDGE_DETECTION_COMPARE_DISTANCE			10.0f
#define LIGHT_MAX_COUNT_IN_TILE 				256

#define TILE_RES 								16
#define TILE_RES_HALF							(TILE_RES / 2)

#if defined(USE_COMPUTE_SHADER)

groupshared uint	s_lightIndexCounter;
groupshared uint	s_lightIdx[LIGHT_MAX_COUNT_IN_TILE];

#endif

uint GetNumTilesX()
{
	return (uint)((tbrParam_viewPortSize.x + TILE_RES - 1) / (float)TILE_RES);
}

uint GetNumTilesY()
{
	return (uint)((tbrParam_viewPortSize.y + TILE_RES - 1) / (float)TILE_RES);
}

float4 ProjToView( float4 p )
{
    p = mul( p, tbrParam_invProjMat );
    p /= p.w;
    return p;
}

float InvertProjDepthToView(float depth)
{
	/*
	1.0f = (depth * tbrParam_invProjMat._33 + tbrParam_invProjMat._43)
	but, tbrParam_invProjMat._33 is always zero and _43 is always 1
		
	if you dont understand, calculate inverse projection matrix.
	but, I use inverted depth writing, so, far value is origin near value and near value is origin far value.
	*/

	return 1.0f / (depth * tbrParam_invProjMat._34 + tbrParam_invProjMat._44);
}

uint GetTileIndex(float2 screenPos)
{
	float tileRes = (float)TILE_RES;

	uint w = GetNumTilesX();
	uint tileIndex = floor(screenPos.x / tileRes) + floor(screenPos.y / tileRes) * w;

	return tileIndex;
}


#endif