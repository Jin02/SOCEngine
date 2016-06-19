//NOT_CREATE_META_DATA

#ifndef __SOC_LIGHT_CULLING_CS_COMPARE_ATOMIC_H__
#define __SOC_LIGHT_CULLING_CS_COMPARE_ATOMIC_H__

#include "LightCullingCommonCS.h"

#define THREAD_COUNT LIGHT_CULLING_TILE_RES * LIGHT_CULLING_TILE_RES

groupshared uint s_minZ;
groupshared uint s_maxZ;

#if (MSAA_SAMPLES_COUNT > 1)
bool CalcDepthBoundMSAA(uint2 globalIdx)
{
	float minZ = FLOAT_MAX;
	float maxZ = 0.0f;

	for(uint sampleIdx=0; sampleIdx<MSAA_SAMPLES_COUNT; ++sampleIdx)
	{
		float opaqueDepth		= Depth.Load(uint2(globalIdx.x, globalIdx.y), sampleIdx).x;
		float opaqueDepthToView		= InvertProjDepthToView(opaqueDepth);
#if defined(ENABLE_BLEND)
		float blendedDepth		= BlendedDepth.Load(uint2(globalIdx.x, globalIdx.y), sampleIdx).x;
		float blendedDepthToView	= InvertProjDepthToView(blendedDepth);

		if(blendedDepth != 0.0f)
		{
			minZ = min(minZ, blendedDepthToView);
			maxZ = max(maxZ, opaqueDepthToView);
		}
#else
		if(opaqueDepth != 0.0f)
		{
			minZ = min(minZ, opaqueDepthToView);
			maxZ = max(maxZ, opaqueDepthToView);
		}
#endif
	}

	uint reinterpretMaxZ = asuint(maxZ);
	uint reinterpretMinZ = asuint(minZ);
	InterlockedMin(s_minZ, reinterpretMinZ);
	InterlockedMax(s_maxZ, reinterpretMaxZ);

	return (maxZ - minZ) > EDGE_DETECTION_COMPARE_DISTANCE;
}
#else
void CalcDepthBound(uint2 globalIdx)
{
	float opaqueDepth		= Depth.Load( uint3(globalIdx.x, globalIdx.y, 0) ).x;
	float opaqueDepthToView		= InvertProjDepthToView(opaqueDepth);
	uint opaqueZ			= asuint(opaqueDepthToView);

#if defined(ENABLE_BLEND)
	float blendedDepth		= BlendedDepth.Load( uint3(globalIdx.x, globalIdx.y, 0) ).x;
	float blendedDepthToView	= InvertProjDepthToView(blendedDepth);

	uint blendedZ = asuint(blendedDepthToView);

	if(blendedDepth != 0.0f)
	{
		InterlockedMin(s_minZ, blendedZ);
		InterlockedMax(s_maxZ, opaqueZ);
	}
#else
	if(opaqueDepth != 0.0f)
	{
		InterlockedMin(s_minZ, opaqueZ);
		InterlockedMax(s_maxZ, opaqueZ);
	}
#endif
}
#endif

#if (MSAA_SAMPLES_COUNT > 1)
bool LightCulling(in uint3 globalIdx, in uint3 localIdx, in uint3 groupIdx, out uint outPointLightCountInTile)
#else
void LightCulling(in uint3 globalIdx, in uint3 localIdx, in uint3 groupIdx, out uint outPointLightCountInTile)
#endif
{
	uint idxInTile	= localIdx.x + localIdx.y * LIGHT_CULLING_TILE_RES;
	
	float4 frustumPlaneNormal[4];
	{
		float2 tl =			float2(	(float)(LIGHT_CULLING_TILE_RES * groupIdx.x),
							(float)(LIGHT_CULLING_TILE_RES * groupIdx.y));
		float2 br =			float2(	(float)(LIGHT_CULLING_TILE_RES * (groupIdx.x + 1)), 
							(float)(LIGHT_CULLING_TILE_RES * (groupIdx.y + 1)));
		float2 totalThreadLength =	float2(	(float)(LIGHT_CULLING_TILE_RES * GetNumTilesX()),	//스크린 픽셀 사이즈라 생각해도 좋고,
							(float)(LIGHT_CULLING_TILE_RES * GetNumTilesY()) );	//현재 돌아가는 전체 가로x세로 스레드 수?
											
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
			frustumPlaneNormal[i] = CreatePlaneNormal(frustum[i], frustum[(i+1) & 3]);
	}

	GroupMemoryBarrierWithGroupSync();

#if (MSAA_SAMPLES_COUNT > 1)
	bool isEdge = CalcDepthBoundMSAA(globalIdx.xy);
#else
	CalcDepthBound(globalIdx.xy);
#endif
	GroupMemoryBarrierWithGroupSync();

	float minZ = asfloat(s_minZ);
	float maxZ = asfloat(s_maxZ);

	uint pointLightCount = GetNumOfPointLight(tbrParam_packedNumOfLights);
    for(uint pointLightIdx=idxInTile; pointLightIdx<pointLightCount; pointLightIdx+=THREAD_COUNT)
    {
		float4 center	= g_inputPointLightTransformBuffer[pointLightIdx];
		float r		= center.w;

		center.xy	= mul( float4(center.xyz, 1), tbrParam_viewMat ).xyz;

		if( ((-center.z + minZ) < r) && ((center.z - maxZ) < r) )
		{
			if( 	InFrustum(center, frustumPlaneNormal[0], r) &&
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

	outPointLightCountInTile	= s_lightIndexCounter;
	uint pointLightCountInTile	= s_lightIndexCounter;

	uint spotLightCount = GetNumOfSpotLight(tbrParam_packedNumOfLights);
	for(uint spotLightIdx=idxInTile; spotLightIdx<spotLightCount; spotLightIdx+=THREAD_COUNT)
	{
		float4 center	= g_inputSpotLightTransformBuffer[spotLightIdx];
		float r		= center.w;

		center.xyz	= mul( float4(center.xyz, 1), tbrParam_viewMat ).xyz;

		if( ((-center.z + minZ) < r) && ((center.z - maxZ) < r) )
		{
			if(	InFrustum(center, frustumPlaneNormal[0], r) &&
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
