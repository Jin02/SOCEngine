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

#if defined(USE_EDGE_CHECK_COMPARE_DISTANCE)
groupshared bool	s_isDetectedEdge[TILE_RES * TILE_RES];
#endif

#endif

uint GetNumOfPointLight()
{
	return (tbrParam_numOfLights & 0xFFE00000) >> 21;
}

uint GetNumOfSpotLight()
{
	return (tbrParam_numOfLights & 0x0001FFC00) >> 10;
}

uint GetNumOfDirectionalLight()
{
	return tbrParam_numOfLights & 0x000007FF;
}

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

float4 CreatePlaneNormal( float4 b, float4 c )
{
    float4 n;
    //b.xyz - a.xyz, c.xyz - a.xyz이다.
    //여기서, a는 원점이다. 즉, ab는 원점에서 해당 타일의 꼭짓점까지 떨어진 방향을 뜻한다.
    n.xyz = normalize(cross( b.xyz, c.xyz ));
    n.w = 0;

    return n;
}

bool InFrustum( float4 p, float4 frusutmNormal, float r )
{
	//여기서 뒤에 + frusutmNormal.w 해야하지만, 이 값은 0이라 더할 필요 없음
	return dot( frusutmNormal.xyz, p.xyz ) < r;
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
	uint tileIndex = (uint)(screenPos.x / tileRes) + ((uint)(screenPos.y / tileRes) * GetNumTilesX());

	return tileIndex;
}


#endif