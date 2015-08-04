//EMPTY_META_DATA

#define THREAD_COUNT 				(TILE_RES_HALF * TILE_RES_HALF)
#define EDGE_DETECTION_VALUE		10.0f
#define TILE_RES 					16
#define TILE_RES_HALF				(TILE_RES / 2)

#define LIGHT_MAX_COUNT_IN_TILE 	544
#define FLOAT_MAX					3.402823466e+38F

#define END_OF_LIGHT_BUFFER			0xffffffff

//Input Buffer
Buffer<float4> 		g_inputPointLightTransformBuffer 	: register(t0);
Buffer<float4> 		g_inputSpotLightTransformBuffer 	: register(t1);


#if (MSAA_SAMPLES_COUNT > 1)
Texture2DMS<float, MSAA_SAMPLES_COUNT>	g_tDepthTexture		: register(t2);
#else
Texture2D<float> 	g_tDepthTexture 						: register(t2);
#endif

#ifdef ENABLE_BLEND

#if (MSAA_SAMPLES_COUNT > 1)
Texture2DMS<float, MSAA_SAMPLES_COUNT>	g_tBlendedDepthTexture		: register(t3);
#else
Texture2D<float> 	g_tBlendedDepthTexture 							: register(t3);
#endif

#endif

groupshared float	s_depthMaxDatas[TILE_RES_HALF * TILE_RES_HALF];
groupshared float	s_depthMinDatas[TILE_RES_HALF * TILE_RES_HALF];

groupshared uint	s_lightIndexCounter;
groupshared uint	s_lightIdx[LIGHT_MAX_COUNT_IN_TILE];

groupshared bool	s_isDetectedEdge[TILE_RES * TILE_RES];

cbuffer LightCullingGlobalData : register( b0 )
{
	matrix	g_viewMat;
	matrix 	g_invProjMat;
	float2	g_screenSize;
	uint 	g_lightNum;
	uint 	g_blockSizeInLightBuffer;
};

uint GetNumTilesX()
{
	return (uint)((g_screenSize.x + TILE_RES - 1) / (float)TILE_RES);
}

uint GetNumTilesY()
{
	return (uint)((g_screenSize.y + TILE_RES - 1) / (float)TILE_RES);
}

float4 ProjToView( float4 p )
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

bool InFrustum( float4 p, float4 frusutmNormal, float r )
{
	//여기서 뒤에 + frusutmNormal.w 해야하지만, 이 값은 0이라 더할 필요 없음
	return dot( frusutmNormal.xyz, p.xyz ) < r;
}

float InvertProjDepthToWorldViewDepth(float depth)
{
	/*
	1.0f = (depth * g_invProjMat._33 + g_invProjMat._43)
	but, g_invProjMat._33 is always zero and _43 is always 1
		
	if you dont understand, calculate inverse projection matrix.
	but, I use inverted depth writing, so, far value is origin near value and near value is origin far value.
	*/

	return 1.0f / (depth * g_invProjMat._34 + g_invProjMat._44);
}

#if (MSAA_SAMPLES_COUNT > 1)
struct CornerMinMax
{
	float min_tl, max_tl;
	float min_tr, max_tr;
	float min_bl, max_bl;
	float min_br, max_br;
};
void CalcMinMax(uint2 halfGlobalIdx, uint2 halfLocalIdx, uint idxInTile, uint depthBufferSamplerIdx, out float outMin, out float outMax, out CornerMinMax ioCornerMinMax)
#else
void CalcMinMax(uint2 halfGlobalIdx, uint idxInTile, uint depthBufferSamplerIdx, out float outMin, out float outMax)
#endif
{
	uint2 idx = halfGlobalIdx * 2;
	
#if (MSAA_SAMPLES_COUNT > 1)
	float depth_tl = g_tDepthTexture.Load( uint2(idx.x,		idx.y),		depthBufferSamplerIdx ).x;
	float depth_tr = g_tDepthTexture.Load( uint2(idx.x+1,	idx.y),		depthBufferSamplerIdx ).x;
	float depth_bl = g_tDepthTexture.Load( uint2(idx.x,		idx.y+1),	depthBufferSamplerIdx ).x;
	float depth_br = g_tDepthTexture.Load( uint2(idx.x+1,	idx.y+1),	depthBufferSamplerIdx ).x;

#if ENABLE_BLEND
	float blendedDepth_tl = g_tBlendedDepthTexture.Load( uint2(idx.x,	idx.y),		depthBufferSamplerIdx ).x;
	float blendedDepth_tr = g_tBlendedDepthTexture.Load( uint2(idx.x+1,	idx.y),		depthBufferSamplerIdx ).x;
	float blendedDepth_br = g_tBlendedDepthTexture.Load( uint2(idx.x+1,	idx.y+1),	depthBufferSamplerIdx ).x;
	float blendedDepth_bl = g_tBlendedDepthTexture.Load( uint2(idx.x,	idx.y+1),	depthBufferSamplerIdx ).x;
#endif

#else
	float depth_tl = g_tDepthTexture.Load( uint3(idx.x,		idx.y,		0)).x;
	float depth_tr = g_tDepthTexture.Load( uint3(idx.x+1,	idx.y,		0)).x;
	float depth_bl = g_tDepthTexture.Load( uint3(idx.x,		idx.y+1,	0)).x;
	float depth_br = g_tDepthTexture.Load( uint3(idx.x+1,	idx.y+1,	0)).x;

#if ENABLE_BLEND
	float blendedDepth_tl = g_tBlendedDepthTexture.Load( uint3(idx.x,	idx.y,		0)).x;
	float blendedDepth_tr = g_tBlendedDepthTexture.Load( uint3(idx.x+1,	idx.y,		0)).x;
	float blendedDepth_br = g_tBlendedDepthTexture.Load( uint3(idx.x+1,	idx.y+1,	0)).x;
	float blendedDepth_bl = g_tBlendedDepthTexture.Load( uint3(idx.x,	idx.y+1,	0)).x;
#endif

#endif

	float viewDepth_tl = InvertProjDepthToWorldViewDepth(depth_tl);
	float viewDepth_tr = InvertProjDepthToWorldViewDepth(depth_tr);
	float viewDepth_bl = InvertProjDepthToWorldViewDepth(depth_bl);
	float viewDepth_br = InvertProjDepthToWorldViewDepth(depth_br);

#if ENABLE_BLEND
	float viewBlendedDepth_tl = InvertProjDepthToWorldViewDepth(blendedDepth_tl);
	float viewBlendedDepth_tr = InvertProjDepthToWorldViewDepth(blendedDepth_tr);
	float viewBlendedDepth_br = InvertProjDepthToWorldViewDepth(blendedDepth_br);
	float viewBlendedDepth_bl = InvertProjDepthToWorldViewDepth(blendedDepth_bl);

	float minDepth_tl = (blendedDepth_tl != 0.0f) ? viewBlendedDepth_tl : FLOAT_MAX;
	float minDepth_tr = (blendedDepth_tr != 0.0f) ? viewBlendedDepth_tr : FLOAT_MAX;
	float minDepth_br = (blendedDepth_br != 0.0f) ? viewBlendedDepth_br : FLOAT_MAX;
	float minDepth_bl = (blendedDepth_bl != 0.0f) ? viewBlendedDepth_bl : FLOAT_MAX;
#else
	float minDepth_tl = (depth_tl != 0.0f) ? viewDepth_tl : FLOAT_MAX;
	float minDepth_tr = (depth_tr != 0.0f) ? viewDepth_tr : FLOAT_MAX;
	float minDepth_bl = (depth_bl != 0.0f) ? viewDepth_bl : FLOAT_MAX;
	float minDepth_br = (depth_br != 0.0f) ? viewDepth_br : FLOAT_MAX;
#endif

	float maxDepth_tl = (depth_tl != 0.0f) ? viewDepth_tl : 0.0f;
	float maxDepth_tr = (depth_tr != 0.0f) ? viewDepth_tr : 0.0f;
	float maxDepth_bl = (depth_bl != 0.0f) ? viewDepth_bl : 0.0f;
	float maxDepth_br = (depth_br != 0.0f) ? viewDepth_br : 0.0f;

#if (MSAA_SAMPLES_COUNT > 1)
	ioCornerMinMax.min_tl = min(minDepth_tl, ioCornerMinMax.min_tl); 	ioCornerMinMax.max_tl = max(maxDepth_tl, ioCornerMinMax.max_tl);
	ioCornerMinMax.min_tr = min(minDepth_tr, ioCornerMinMax.min_tr); 	ioCornerMinMax.max_tr = max(maxDepth_tr, ioCornerMinMax.max_tr);
	ioCornerMinMax.min_bl = min(minDepth_bl, ioCornerMinMax.min_bl); 	ioCornerMinMax.max_bl = max(maxDepth_bl, ioCornerMinMax.max_bl);
	ioCornerMinMax.min_br = min(minDepth_br, ioCornerMinMax.min_br); 	ioCornerMinMax.max_br = max(maxDepth_br, ioCornerMinMax.max_br);
#else //Non-MSAA, MSAA처리 시, 여기서하는 edge 검사는 밖에서 함.
	uint2 localIdx = halfLocalIdx * 2;
	uint idxInOriginTile = localIdx.x + localIdx.y * TILE_RES;

	s_isDetectedEdge[idxInOriginTile]					= (maxDepth_tl - minDepth_tl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + 1]				= (maxDepth_tr - minDepth_tr) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + TILE_RES]		= (maxDepth_bl - minDepth_bl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + TILE_RES + 1]	= (maxDepth_br - minDepth_br) > EDGE_DETECTION_VALUE;
#endif

	s_depthMinDatas[idxInTile] = min( minDepth_tl, min(minDepth_tr, min(minDepth_bl, minDepth_br)) );	
	s_depthMaxDatas[idxInTile] = max( maxDepth_tl, max(maxDepth_tr, max(maxDepth_bl, maxDepth_br)) );

	GroupMemoryBarrierWithGroupSync();

	//반만 하면 됨
	if( idxInTile < 32 )
	{
		s_depthMinDatas[idxInTile] = min( s_depthMinDatas[idxInTile], s_depthMinDatas[idxInTile + 32] );
		s_depthMinDatas[idxInTile] = min( s_depthMinDatas[idxInTile], s_depthMinDatas[idxInTile + 16] );
		s_depthMinDatas[idxInTile] = min( s_depthMinDatas[idxInTile], s_depthMinDatas[idxInTile + 8] );
		s_depthMinDatas[idxInTile] = min( s_depthMinDatas[idxInTile], s_depthMinDatas[idxInTile + 4] );
		s_depthMinDatas[idxInTile] = min( s_depthMinDatas[idxInTile], s_depthMinDatas[idxInTile + 2] );
		s_depthMinDatas[idxInTile] = min( s_depthMinDatas[idxInTile], s_depthMinDatas[idxInTile + 1] );

		s_depthMaxDatas[idxInTile] = max( s_depthMaxDatas[idxInTile], s_depthMaxDatas[idxInTile + 32] );
		s_depthMaxDatas[idxInTile] = max( s_depthMaxDatas[idxInTile], s_depthMaxDatas[idxInTile + 16] );
		s_depthMaxDatas[idxInTile] = max( s_depthMaxDatas[idxInTile], s_depthMaxDatas[idxInTile + 8] );
		s_depthMaxDatas[idxInTile] = max( s_depthMaxDatas[idxInTile], s_depthMaxDatas[idxInTile + 4] );
		s_depthMaxDatas[idxInTile] = max( s_depthMaxDatas[idxInTile], s_depthMaxDatas[idxInTile + 2] );
		s_depthMaxDatas[idxInTile] = max( s_depthMaxDatas[idxInTile], s_depthMaxDatas[idxInTile + 1] );
	}

	GroupMemoryBarrierWithGroupSync();

	outMin = s_depthMinDatas[0];
	outMax = s_depthMaxDatas[0];
}

bool ClacMinMaxAndCheckEdgeDetection(uint2 halfGlobalIdx, uint2 halfLocalIdx, uint idxInTile, out float outMin, out float outMax)
{
	float minZ = FLOAT_MAX;
	float maxZ = 0.0f;

#if (MSAA_SAMPLES_COUNT > 1)
	uint2 depthBufferSize;
	uint depthBufferSampleCount;

	g_tDepthTexture.GetDimensions(depthBufferSize.x, depthBufferSize.y, depthBufferSampleCount);

	float tmpMin = FLOAT_MAX;
	float tmpMax = 0.0f;

	CornerMinMax cornerMinMax;
	{
		cornerMinMax.min_tl = FLOAT_MAX;	cornerMinMax.min_tr = FLOAT_MAX;	cornerMinMax.min_bl = FLOAT_MAX;	cornerMinMax.min_br = FLOAT_MAX;
		cornerMinMax.max_tl = 0;			cornerMinMax.max_tr = 0;			cornerMinMax.max_bl = 0;			cornerMinMax.max_br = 0;
	}

	for(uint sampleIdx=0; sampleIdx<depthBufferSampleCount; ++sampleIdx)
	{
		CalcMinMax(halfGlobalIdx, halfLocalIdx, idxInTile, sampleIdx, tmpMin, tmpMax, minMax, cornerMinMax);

		minZ = min(tmpMin, minZ);
		maxZ = max(tmpMax, maxZ);
	}

	uint2 localIdx = halfLocalIdx * 2;
	uint idxInOriginTile = localIdx.x + localIdx.y * TILE_RES;

	s_isDetectedEdge[idxInOriginTile]					= (cornerMinMax.max_tl - cornerMinMax.min_tl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + 1]				= (cornerMinMax.max_tr - cornerMinMax.min_tr) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + TILE_RES]		= (cornerMinMax.max_bl - cornerMinMax.min_bl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + TILE_RES + 1]	= (cornerMinMax.max_br - cornerMinMax.min_br) > EDGE_DETECTION_VALUE;

#else // Non-MSAA
	CalcMinMax(halfGlobalIdx, idxInTile, 0, minZ, maxZ);
#endif

	GroupMemoryBarrierWithGroupSync();

	outMin = minZ;
	outMax = maxZ;
}

void LightCulling(in uint3 halfGlobalIdx, in uint3 halfLocalIdx, in uint3 groupIdx, out uint outPointLightCountInTile, out float minZ, out float maxZ)
{
	uint idxInTile	= halfLocalIdx.x + halfLocalIdx.y * TILE_RES_HALF;
	uint idxOfGroup	= groupIdx.x + groupIdx.y * GetNumTilesX();
	
	//한번만 초기화
	if(idxInTile == 0)
		s_lightIndexCounter	= 0;

	float4 frustumPlaneNormal[4];
	{
		uint2 tl =					uint2(	TILE_RES * groupIdx.x,
											TILE_RES * groupIdx.y);
		uint2 br =					uint2(	TILE_RES * (groupIdx.x + 1), 
											TILE_RES * (groupIdx.y + 1));
		float2 totalThreadLength =	float2(	(float)(TILE_RES*GetNumTilesX()),
											(float)(TILE_RES*GetNumTilesY()) );
										//스크린 픽셀 사이즈라 생각해도 좋고,
										//현재 돌아가는 전체 가로x세로 스레드 수?
		float4 frustum[4];
		frustum[0] = ProjToView( float4( tl.x / totalThreadLength.x * 2.f - 1.f, 
											   (totalThreadLength.y - tl.y) / totalThreadLength.y * 2.f - 1.f,
												1.f, 1.f) ); //TL
		frustum[1] = ProjToView( float4( br.x / totalThreadLength.x * 2.f - 1.f, 
												(totalThreadLength.y - tl.y) / totalThreadLength.y * 2.f - 1.f,
												1.f, 1.f) ); //TR
		frustum[2] = ProjToView( float4( br.x / totalThreadLength.x * 2.f - 1.f, 
												(totalThreadLength.y - br.y) / totalThreadLength.y * 2.f - 1.f,
												1.f, 1.f) ); //BR
		frustum[3] = ProjToView( float4( tl.x / totalThreadLength.x * 2.f - 1.f, 
												(totalThreadLength.y - br.y) / totalThreadLength.y * 2.f - 1.f,
												1.f, 1.f) ); //BL

		for(uint i=0; i<4; ++i)
			frustumPlaneNormal[i] = CreatePlaneNormal(frustum[i], frustum[(i+1) % 4]);
	}

	GroupMemoryBarrierWithGroupSync();

	minZ = FLOAT_MAX;
	maxZ = 0.0f;

	ClacMinMaxAndCheckEdgeDetection(halfGlobalIdx.xy, halfLocalIdx.xy, idxInTile, minZ, maxZ);
	//GroupMemoryBarrierWithGroupSync

    uint pointLightCount = g_lightNum & 0x0000FFFF;
    for(uint i=0; i<pointLightCount; i+=THREAD_COUNT)
    {
        uint lightIdx = idxInTile + i;
        if( lightIdx < pointLightCount )
        {
			float4 center = g_inputPointLightTransformBuffer[lightIdx];
			float r = center.w;
		
			center.xyz = mul( float4(center.xyz, 1), g_viewMat ).xyz;

			if( ((-center.z + minZ) < r) && ((center.z - maxZ) < r) )
			{
				if( InFrustum(center, frustumPlaneNormal[0], r) &&
					InFrustum(center, frustumPlaneNormal[1], r) &&
					InFrustum(center, frustumPlaneNormal[2], r) &&
					InFrustum(center, frustumPlaneNormal[3], r) )
				{
					uint target = 0;
					InterlockedAdd(s_lightIndexCounter, 1, target);

					if(target < LIGHT_MAX_COUNT_IN_TILE)
						s_lightIdx[target] = lightIdx;
				}
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();
	uint pointLightCountInTile = s_lightIndexCounter;
	outPointLightCountInTile = pointLightCountInTile;

	uint spotLightCount = (g_lightNum & 0xFFFF0000) >> 16;
	for(uint j=0; j<spotLightCount; j+=THREAD_COUNT)
	{
		uint lightIdx = idxInTile + j;
		if( lightIdx < spotLightCount )
		{
			float4 center = g_inputSpotLightTransformBuffer[lightIdx];
			float r = center.w;
			
			center.xyz = mul( float4(center.xyz, 1), g_viewMat ).xyz;

			if( ((-center.z + minZ) < r) && ((center.z - maxZ) < r) )
			{
				if( InFrustum(center, frustumPlaneNormal[0], r) &&
					InFrustum(center, frustumPlaneNormal[1], r) &&
					InFrustum(center, frustumPlaneNormal[2], r) &&
					InFrustum(center, frustumPlaneNormal[3], r) )
				{
					uint target = 0;
					InterlockedAdd(s_lightIndexCounter, 1, target);

					if(target < LIGHT_MAX_COUNT_IN_TILE)
						s_lightIdx[target] = lightIdx;
				}
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();
}