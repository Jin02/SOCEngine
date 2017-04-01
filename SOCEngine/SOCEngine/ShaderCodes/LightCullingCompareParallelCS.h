//NOT_CREATE_META_DATA

#ifndef __SOC_LIGHT_CULLING_CS_COMPARE_PARALLEL_H__
#define __SOC_LIGHT_CULLING_CS_COMPARE_PARALLEL_H__

#include "LightCullingCommonCS.h"

#define THREAD_COUNT TILE_RES_HALF*TILE_RES_HALF

groupshared float	s_depthMaxDatas[TILE_RES_HALF * TILE_RES_HALF];
groupshared float	s_depthMinDatas[TILE_RES_HALF * TILE_RES_HALF];

#if defined(USE_EDGE_CHECK_COMPARE_DISTANCE)
groupshared bool	s_isDetectedEdge[LIGHT_CULLING_TILE_RES * LIGHT_CULLING_TILE_RES];
#endif

#if (MSAA_SAMPLES_COUNT > 1) && defined(USE_EDGE_CHECK_COMPARE_DISTANCE)
struct CornerMinMax
{
	float min_tl, max_tl;
	float min_tr, max_tr;
	float min_bl, max_bl;
	float min_br, max_br;
};
void CalcMinMax(uint2 halfGlobalIdx, uint2 halfLocalIdx, uint idxInTile, uint depthBufferSamplerIdx, out float outMin, out float outMax, inout CornerMinMax ioCornerMinMax)
#else
void CalcMinMax(uint2 halfGlobalIdx, uint2 halfLocalIdx, uint idxInTile, uint depthBufferSamplerIdx, out float outMin, out float outMax)
#endif
{
	uint2 idx = halfGlobalIdx * 2;
	
#if (MSAA_SAMPLES_COUNT > 1)
	float depth_tl = GBufferDepth.Load( uint2(idx.x,	idx.y),		depthBufferSamplerIdx ).x;
	float depth_tr = GBufferDepth.Load( uint2(idx.x+1,	idx.y),		depthBufferSamplerIdx ).x;
	float depth_bl = GBufferDepth.Load( uint2(idx.x,	idx.y+1),	depthBufferSamplerIdx ).x;
	float depth_br = GBufferDepth.Load( uint2(idx.x+1,	idx.y+1),	depthBufferSamplerIdx ).x;

#if defined(ENABLE_BLEND)
	float blendedDepth_tl = GBufferBlendedDepth.Load( uint2(idx.x,	 idx.y),	depthBufferSamplerIdx ).x;
	float blendedDepth_tr = GBufferBlendedDepth.Load( uint2(idx.x+1, idx.y),	depthBufferSamplerIdx ).x;
	float blendedDepth_br = GBufferBlendedDepth.Load( uint2(idx.x+1, idx.y+1),	depthBufferSamplerIdx ).x;
	float blendedDepth_bl = GBufferBlendedDepth.Load( uint2(idx.x,	 idx.y+1),	depthBufferSamplerIdx ).x;
#endif

#else
	float depth_tl = GBufferDepth.Load( uint3(idx.x,	idx.y,		0) ).x;
	float depth_tr = GBufferDepth.Load( uint3(idx.x+1,	idx.y,		0) ).x;
	float depth_bl = GBufferDepth.Load( uint3(idx.x,	idx.y+1,	0) ).x;
	float depth_br = GBufferDepth.Load( uint3(idx.x+1,	idx.y+1,	0) ).x;

#if defined(ENABLE_BLEND)
	float blendedDepth_tl = GBufferBlendedDepth.Load( uint3(idx.x,	 idx.y,		0) ).x;
	float blendedDepth_tr = GBufferBlendedDepth.Load( uint3(idx.x+1, idx.y,		0) ).x;
	float blendedDepth_br = GBufferBlendedDepth.Load( uint3(idx.x+1, idx.y+1,	0) ).x;
	float blendedDepth_bl = GBufferBlendedDepth.Load( uint3(idx.x,	 idx.y+1,	0) ).x;
#endif

#endif

	float viewDepth_tl = InvertProjDepthToView(depth_tl);
	float viewDepth_tr = InvertProjDepthToView(depth_tr);
	float viewDepth_bl = InvertProjDepthToView(depth_bl);
	float viewDepth_br = InvertProjDepthToView(depth_br);

#if defined(ENABLE_BLEND)
	float viewBlendedDepth_tl = InvertProjDepthToView(blendedDepth_tl);
	float viewBlendedDepth_tr = InvertProjDepthToView(blendedDepth_tr);
	float viewBlendedDepth_br = InvertProjDepthToView(blendedDepth_br);
	float viewBlendedDepth_bl = InvertProjDepthToView(blendedDepth_bl);

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

#if defined(USE_EDGE_CHECK_COMPARE_DISTANCE)

#if (MSAA_SAMPLES_COUNT > 1)
	ioCornerMinMax.min_tl = min(minDepth_tl, ioCornerMinMax.min_tl); 	ioCornerMinMax.max_tl = max(maxDepth_tl, ioCornerMinMax.max_tl);
	ioCornerMinMax.min_tr = min(minDepth_tr, ioCornerMinMax.min_tr); 	ioCornerMinMax.max_tr = max(maxDepth_tr, ioCornerMinMax.max_tr);
	ioCornerMinMax.min_bl = min(minDepth_bl, ioCornerMinMax.min_bl); 	ioCornerMinMax.max_bl = max(maxDepth_bl, ioCornerMinMax.max_bl);
	ioCornerMinMax.min_br = min(minDepth_br, ioCornerMinMax.min_br); 	ioCornerMinMax.max_br = max(maxDepth_br, ioCornerMinMax.max_br);
#else // Edge 체크, MSAA는 밖에서 처리함
	uint2 localIdx = halfLocalIdx * 2;
	uint idxInOriginTile = localIdx.x + localIdx.y * LIGHT_CULLING_TILE_RES;

	s_isDetectedEdge[idxInOriginTile]								= (maxDepth_tl - minDepth_tl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + 1]							= (maxDepth_tr - minDepth_tr) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + LIGHT_CULLING_TILE_RES]		= (maxDepth_bl - minDepth_bl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + LIGHT_CULLING_TILE_RES + 1]	= (maxDepth_br - minDepth_br) > EDGE_DETECTION_VALUE;
#endif

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

void ClacMinMaxWithCheckEdgeDetection(uint2 halfGlobalIdx, uint2 halfLocalIdx, uint idxInTile, out float outMin, out float outMax)
{
	float minZ = FLOAT_MAX;
	float maxZ = 0.0f;

#if (MSAA_SAMPLES_COUNT > 1)
	float tmpMin = FLOAT_MAX;
	float tmpMax = 0.0f;

#if defined(USE_EDGE_CHECK_COMPARE_DISTANCE)
	CornerMinMax cornerMinMax;
	{
		cornerMinMax.min_tl = FLOAT_MAX;	cornerMinMax.min_tr = FLOAT_MAX;	cornerMinMax.min_bl = FLOAT_MAX;	cornerMinMax.min_br = FLOAT_MAX;
		cornerMinMax.max_tl = 0;			cornerMinMax.max_tr = 0;			cornerMinMax.max_bl = 0;			cornerMinMax.max_br = 0;
	}
#endif

	for(uint sampleIdx=0; sampleIdx<MSAA_SAMPLES_COUNT; ++sampleIdx)
	{
#if defined(USE_EDGE_CHECK_COMPARE_DISTANCE)
		CalcMinMax(halfGlobalIdx, halfLocalIdx, idxInTile, sampleIdx, tmpMin, tmpMax, cornerMinMax);
#else
		CalcMinMax(halfGlobalIdx, halfLocalIdx, idxInTile, sampleIdx, tmpMin, tmpMax);
#endif

		minZ = min(tmpMin, minZ);
		maxZ = max(tmpMax, maxZ);
	}

#if defined(USE_EDGE_CHECK_COMPARE_DISTANCE)
	uint2 localIdx = halfLocalIdx * 2;
	uint idxInOriginTile = localIdx.x + localIdx.y * LIGHT_CULLING_TILE_RES;

	s_isDetectedEdge[idxInOriginTile]								= (cornerMinMax.max_tl - cornerMinMax.min_tl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + 1]							= (cornerMinMax.max_tr - cornerMinMax.min_tr) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + LIGHT_CULLING_TILE_RES]		= (cornerMinMax.max_bl - cornerMinMax.min_bl) > EDGE_DETECTION_VALUE;
	s_isDetectedEdge[idxInOriginTile + LIGHT_CULLING_TILE_RES + 1]	= (cornerMinMax.max_br - cornerMinMax.min_br) > EDGE_DETECTION_VALUE;
#endif

#else // Non-MSAA
	CalcMinMax(halfGlobalIdx, halfLocalIdx, idxInTile, 0, minZ, maxZ);
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
		uint2 tl =					uint2(	LIGHT_CULLING_TILE_RES * groupIdx.x,
											LIGHT_CULLING_TILE_RES * groupIdx.y);
		uint2 br =					uint2(	LIGHT_CULLING_TILE_RES * (groupIdx.x + 1), 
											LIGHT_CULLING_TILE_RES * (groupIdx.y + 1));
		float2 totalThreadLength =	float2(	(float)(LIGHT_CULLING_TILE_RES * GetNumTilesX()),
											(float)(LIGHT_CULLING_TILE_RES * GetNumTilesY()) );
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

	ClacMinMaxWithCheckEdgeDetection(halfGlobalIdx.xy, halfLocalIdx.xy, idxInTile, minZ, maxZ);
	GroupMemoryBarrierWithGroupSync();

	uint pointLightCount = GetNumOfPointLight();
    for(uint pointLightIdx=idxInTile; pointLightIdx<pointLightCount; pointLightIdx+=THREAD_COUNT)
    {
		float4 center = PointLightTransformBuffer[pointLightIdx];
		float r = center.w;

		center.xyz = mul( float4(center.xyz, 1), camera_viewMat ).xyz;

		if( ((-center.z + minZ) < r) && ((center.z - maxZ) < r) )
		{
			if( InFrustum(center, frustumPlaneNormal[0], r) &&
				InFrustum(center, frustumPlaneNormal[1], r) &&
				InFrustum(center, frustumPlaneNormal[2], r) &&
				InFrustum(center, frustumPlaneNormal[3], r) )
			{
				uint target = 0;
				InterlockedAdd(s_lightIndexCounter, 1, target);

				s_lightIdx[target] = pointLightIdx;
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	uint pointLightCountInTile = s_lightIndexCounter;
	outPointLightCountInTile = pointLightCountInTile;

	uint spotLightCount = GetNumOfSpotLight();
	for(uint spotLightIdx=idxInTile; spotLightIdx<spotLightCount; spotLightIdx+=THREAD_COUNT)
	{
		float4 center = SpotLightTransformBuffer[spotLightIdx];
		float r = center.w;

		center.xyz = mul( float4(center.xyz, 1), camera_viewMat ).xyz;

		if( ((-center.z + minZ) < r) && ((center.z - maxZ) < r) )
		{
			if( InFrustum(center, frustumPlaneNormal[0], r) &&
				InFrustum(center, frustumPlaneNormal[1], r) &&
				InFrustum(center, frustumPlaneNormal[2], r) &&
				InFrustum(center, frustumPlaneNormal[3], r) )
			{
				uint target = 0;
				InterlockedAdd(s_lightIndexCounter, 1, target);

				s_lightIdx[target] = spotLightIdx;
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();
}

#endif
