//NOT_CREATE_META_DATA

#ifndef __SOC_LIGHT_CULLING_CS_COMPARE_ATOMIC_H__
#define __SOC_LIGHT_CULLING_CS_COMPARE_ATOMIC_H__

#include "LightCullingCommonCS.h"

#define THREAD_COUNT TILE_RES * TILE_RES

#if defined(USE_COMPUTE_SHADER)
RWBuffer<uint> g_outPerLightIndicesInTile	: register( u0 );

groupshared int s_minZ;
groupshared int s_maxZ;
#endif

#if (MSAA_SAMPLES_COUNT > 1)
void CalcMinMax(uint2 globalIdx, uint depthBufferSamplerIdx, inout float ioMinZ, inout float ioMaxZ)
#else
void CalcMinMax(uint2 globalIdx, inout float ioMinZ, inout float ioMaxZ)
#endif
{
#if (MSAA_SAMPLES_COUNT > 1)
	float depth = g_tDepth.Load( uint2(globalIdx.x, globalIdx.y), depthBufferSamplerIdx ).x;
#if defined(ENABLE_BLEND)
	float blendedDepth = g_tBlendedDepth.Load( uint2(globalIdx.x, globalIdx.y), depthBufferSamplerIdx ).x;
#endif
#else
	float depth = g_tDepth.Load( uint3(globalIdx.x, globalIdx.y, 0) ).x;
#if defined(ENABLE_BLEND)
	float blendedDepth = g_tBlendedDepth.Load( uint3(globalIdx.x, globalIdx.y, 0) ).x;
#endif
#endif

	float opaqueViewDepth = InvertProjDepthToView(depth);

#if defined(ENABLE_BLEND)
	float blendedViewDepth = InvertProjDepthToView(blendedDepth);

	if(blendedViewDepth != 0.0f)
	{
		ioMinZ = min(ioMinZ, blendedViewDepth);
		ioMaxZ = max(ioMaxZ, opaqueViewDepth);
	}
#else
	if(opaqueViewDepth != 0.0f)
	{
		ioMinZ = min(ioMinZ, opaqueViewDepth);
		ioMaxZ = max(ioMaxZ, opaqueViewDepth);
	}
#endif
}

#if (MSAA_SAMPLES_COUNT > 1)
bool CalcMinMaxDepthWithCheckEdgeDetection(uint2 globalIdx, out float outMin, out float outMax)
#else
void CalcMinMaxDepth(uint2 globalIdx, out float outMin, out float outMax)
#endif
{
	float minZ = FLOAT_MAX;
	float maxZ = 0.0f;

#if (MSAA_SAMPLES_COUNT > 1)
	for(uint sampleIdx=0; sampleIdx<MSAA_SAMPLES_COUNT; ++sampleIdx)
		CalcMinMax(globalIdx, sampleIdx, minZ, maxZ);
#else // Non-MSAA
	CalcMinMax(globalIdx, minZ, maxZ);
#endif

	uint reinterpretMinZ = asuint(minZ);
	InterlockedMin(s_minZ, reinterpretMinZ);

	uint reinterpretMaxZ = asuint(maxZ);
	InterlockedMax(s_maxZ, reinterpretMaxZ);

	outMin = minZ;
	outMax = maxZ;

#if (MSAA_SAMPLES_COUNT > 1)
	return (maxZ - minZ) > EDGE_DETECTION_COMPARE_DISTANCE;
#endif
}

#if (MSAA_SAMPLES_COUNT > 1)
bool LightCulling(in uint3 globalIdx, in uint3 localIdx, in uint3 groupIdx, out uint outPointLightCountInTile, out float minZ, out float maxZ)
#else
void LightCulling(in uint3 globalIdx, in uint3 localIdx, in uint3 groupIdx, out uint outPointLightCountInTile, out float minZ, out float maxZ)
#endif
{
	uint idxInTile	= localIdx.x + localIdx.y * TILE_RES;
	uint idxOfGroup	= groupIdx.x + groupIdx.y * GetNumTilesX();
	
	//한번만 초기화
	if(idxInTile == 0)
	{
		s_lightIndexCounter	= 0;
		s_minZ = 0;
		s_maxZ = 0;
	}

	float4 frustumPlaneNormal[4];
	{
		uint2 tl =					uint2(	TILE_RES * groupIdx.x,
											TILE_RES * groupIdx.y);
		uint2 br =					uint2(	TILE_RES * (groupIdx.x + 1), 
											TILE_RES * (groupIdx.y + 1));
		float2 totalThreadLength =	float2(	(float)(TILE_RES * GetNumTilesX()),
											(float)(TILE_RES * GetNumTilesY()) );
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

#if (MSAA_SAMPLES_COUNT > 1)
	bool isEdge = CalcMinMaxDepthWithCheckEdgeDetection(globalIdx.xy, minZ, maxZ);
#else
	CalcMinMaxDepth(globalIdx.xy, minZ, maxZ);
#endif
	GroupMemoryBarrierWithGroupSync();

	minZ = asfloat(s_minZ);
	maxZ = asfloat(s_maxZ);

	uint pointLightCount = GetNumOfPointLight();
    for(uint pointLightIdx=idxInTile; pointLightIdx<pointLightCount; pointLightIdx+=THREAD_COUNT)
    {
		float4 center = g_inputPointLightTransformBuffer[pointLightIdx];
		float r = center.w;

		center.xyz = mul( float4(center.xyz, 1), tbrParam_viewMat ).xyz;

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
		float4 center = g_inputSpotLightTransformBuffer[spotLightIdx];
		float r = center.w;

		center.xyz = mul( float4(center.xyz, 1), tbrParam_viewMat ).xyz;

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

#if (MSAA_SAMPLES_COUNT > 1)
	return isEdge;
#endif
}

#endif