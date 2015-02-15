//NOT_CREATE_META_DATA

#define TILE_RES 						16
#define MAX_LIGHT_PER_TILE_NUM 			544
#define MAX_LIGHT_NUM					2048
#define FLOAT_MAX						3.402823466e+38F
>>>>>>> #45

Buffer<float4> 		g_bPointLightCenterWithRadius 	: register(t0);
Buffer<float4> 		g_bSpotLightCenterWithRadius 	: register(t1);
Texture2D<float> 	g_tDepthTexture 				: register(t2);
RWBuffer<uint> 		g_orwbTileLightIndex 			: register(u0);

cbuffer LightCullingGlobalData : register( b0 )
{
	matrix	g_worldViewMat;
	matrix 	g_invProjMat;
	float2	g_screenSize;
	float 	g_clippingFar;
	uint 	g_maxLightNumInTile; //한 타일당 최대 빛 갯수.
	uint 	g_lightNum;
	uint3	dummy;	
};

uint GetNumTilesX()
{
	return (uint)((g_screenSize.x + TILE_RES - 1) / (float)TILE_RES);
}

uint GetNumTilesY()
{
	return (uint)((g_screenSize.y + TILE_RES - 1) / (float)TILE_RES);
}

float4 ConvertProjToView( float4 p )
{
    p = mul( p, g_invProjMat );
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

float InFrustum( float4 p, float4 frusutmNormal )
{
	//여기서 뒤에 + frusutmNormal.w 해야하지만, 이 값은 0이라 더할 필요 없음
	return dot( frusutmNormal.xyz, p.xyz );
}

groupshared uint s_zMax;
groupshared uint s_zMin;

groupshared uint s_lightIndexCounter;
groupshared uint s_lightIdx[MAX_LIGHT_PER_TILE_NUM];

[numthreads(TILE_RES, TILE_RES, 1)]
void LightCullingCS(uint3 globalIdx : SV_DispatchThreadID, uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID)
{
	uint idxInTile	= localIdx.x + localIdx.y * TILE_RES;
	uint idxOfGroup	= groupIdx.x + groupIdx.y * GetNumTilesX();

	//한번만 초기화
	if(idxInTile == 0)
	{
		s_zMin				= 0xffffffff;
		s_zMax 				= 0;
		s_lightIndexCounter	= 0;
	}

	float4 frustumPlaneNormal[4];
	{
		uint2 tl = uint2(	TILE_RES * groupIdx.x,
							TILE_RES * groupIdx.y);
		uint2 br = uint2(	TILE_RES * (groupIdx.x + 1), 
							TILE_RES * (groupIdx.y + 1));
		uint2 totalThreadLength = uint2(TILE_RES*GetNumTilesX(),
										TILE_RES*GetNumTilesY());
										//스크린 픽셀 사이즈라 생각해도 좋고,
										//현재 돌아가는 전체 가로x세로 스레드 수?
		float4 frustum[4];
		frustum[0] = ConvertProjToView( float4( tl.x/(float)totalThreadLength.x * 2.f - 1.f, 
											   (totalThreadLength.y-tl.y)/(float)totalThreadLength.y * 2.f - 1.f,
											   1.f, 1.f) ); //TL
		frustum[1] = ConvertProjToView( float4( br.x/(float)totalThreadLength.x * 2.f - 1.f, 
												(totalThreadLength.y-tl.y)/(float)totalThreadLength.y * 2.f - 1.f,
											   1.f,1.f) ); //TR
		frustum[2] = ConvertProjToView( float4( br.x/(float)totalThreadLength.x * 2.f - 1.f, 
												(totalThreadLength.y-br.y)/(float)totalThreadLength.y * 2.f - 1.f,
												1.f,1.f) ); //BR
		frustum[3] = ConvertProjToView( float4( tl.x/(float)totalThreadLength.x * 2.f - 1.f, 
												(totalThreadLength.y-br.y)/(float)totalThreadLength.y * 2.f - 1.f,
												1.f,1.f) ); //BL

		for(uint i=0; i<4; ++i)
			frustumPlaneNormal[i] = CreatePlaneNormal(frustum[i], frustum[(i+1) % 4]);
	}

	//스레드 싱크 맞추기
	GroupMemoryBarrierWithGroupSync();

	float minZ = FLOAT_MAX;
	float maxZ = 0.0f;

	//shared min, max 계산
	{
		float depth = g_tDepthTexture.Load( uint3(globalIdx.x, globalIdx.y, 0) ).x;

		//non linear depth to viewPos
		uint z = asuint( depth * g_clippingFar );

		if( depth != 0.f )
		{
			//Shared memory에 있는 값과 입력된 값을 비교하여, 결과를 dest에 저장한다.
			//InterlockedMax(__in R dest, __in T value, __out original_value)
			//음수로 가지 않는 한, float를 bit로 변환한다고 하여도 크기의 순위는 변하지 않음.
			InterlockedMax( s_zMax, z );
			InterlockedMin( s_zMin, z );
		}
	}

	GroupMemoryBarrierWithGroupSync();

    maxZ = asfloat( s_zMax );	//uint to float
    minZ = asfloat( s_zMin );

    uint pointLightCount = g_lightNum & 0x0000FFFF;
    for(uint i=0; i<pointLightCount; i+=(TILE_RES * TILE_RES))
    {
    	//idxInTile은, 0~tile_res^2 의 값을 가짐.
        uint lightIdx = idxInTile + i;
        if( lightIdx < pointLightCount )
        {
            float4 center = g_bPointLightCenterWithRadius[lightIdx];
            float r = center.w;
            center.xyz = mul( float4(center.xyz, 1), g_worldViewMat ).xyz;

            if( -center.z + minZ < r && center.z - maxZ < r )
            {
				if( ( InFrustum( center, frustumPlaneNormal[0] ) < r ) && ( InFrustum( center, frustumPlaneNormal[1] ) < r ) && ( InFrustum( center, frustumPlaneNormal[2] ) < r ) && ( InFrustum( center, frustumPlaneNormal[3] ) < r ) )
                {
					uint outIdx = 0;
					InterlockedAdd( s_lightIndexCounter, 1, outIdx );
					s_lightIdx[outIdx] = lightIdx;
				}
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();
	uint pointLightNumInTiles = s_lightIndexCounter; //싱크 후, 결과 저장

	uint spotLightCount = (g_lightNum & 0xFFFF0000) >> 16;
	for(uint j=0; j<spotLightCount; j+=(TILE_RES * TILE_RES))
	{
		uint lightIdx = idxInTile + j;
		if( lightIdx < spotLightCount )
		{
			float4 center = g_bSpotLightCenterWithRadius[lightIdx];
			float r = center.w;
			center.xyz = mul( float4(center.xyz, 1), g_worldViewMat ).xyz;

			if( -center.z + minZ < r && center.z - maxZ < r )
			{
				if( ( InFrustum( center, frustumPlaneNormal[0] ) < r ) && ( InFrustum( center, frustumPlaneNormal[1] ) < r ) && ( InFrustum( center, frustumPlaneNormal[2] ) < r ) && ( InFrustum( center, frustumPlaneNormal[3] ) < r ) )
				{
					uint outIdx = 0;
					InterlockedAdd( s_lightIndexCounter, 1, outIdx );
					s_lightIdx[outIdx] = lightIdx;
				}
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	{
		uint startOffset = g_maxLightNumInTile * idxOfGroup;

		for(uint i=idxInTile; i<pointLightNumInTiles; i+=(TILE_RES * TILE_RES))
			g_orwbTileLightIndex[startOffset + i] = s_lightIdx[i];

		for(uint j=(idxInTile+pointLightNumInTiles); j<s_lightIndexCounter; j+=(TILE_RES * TILE_RES))
			g_orwbTileLightIndex[startOffset + j + 1] = s_lightIdx[j];

		if( idxInTile == 0 )
		{
			g_orwbTileLightIndex[startOffset + pointLightNumInTiles] = 0;
			g_orwbTileLightIndex[startOffset + s_lightIndexCounter + 1] = 0;
		}
	}

}