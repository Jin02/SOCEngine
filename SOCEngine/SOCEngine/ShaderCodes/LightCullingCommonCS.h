//NOT_CREATE_META_DATA

#ifndef __SOC_LIGHT_CULLING_CS_COMMON_H__
#define __SOC_LIGHT_CULLING_CS_COMMON_H__

#include "ShaderCommon.h"
#include "TBDRInput.h"
#include "TBRParam.h"
#include "CommonConstBuffer.h"

#define EDGE_DETECTION_COMPARE_DISTANCE			10.0f
#define TILE_RES_HALF							(LIGHT_CULLING_TILE_RES / 2)

#if defined(USE_COMPUTE_SHADER)

groupshared uint	s_lightIndexCounter;
groupshared uint	s_lightIdx[LIGHT_CULLING_LIGHT_MAX_COUNT_IN_TILE];

#endif

uint GetNumTilesX()
{
	return (uint)((GetViewportSize().x + LIGHT_CULLING_TILE_RES - 1) / (float)LIGHT_CULLING_TILE_RES);
}

uint GetNumTilesY()
{
	return (uint)((GetViewportSize().y + LIGHT_CULLING_TILE_RES - 1) / (float)LIGHT_CULLING_TILE_RES);
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
	return (dot( frusutmNormal.xyz, p.xyz )/*+ frusutmNormal.w*/ < r);
}

uint GetTileIndex(float2 screenPos)
{
	float tileRes = (float)LIGHT_CULLING_TILE_RES;

	uint w = GetNumTilesX();
	uint tileIndex = floor(screenPos.x / tileRes) + floor(screenPos.y / tileRes) * w;

	return tileIndex;
}


#endif
