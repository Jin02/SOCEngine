//NOT_CREATE_META_DATA

#ifndef __SOC_GBUFFER_COMMON_H__
#define __SOC_GBUFFER_COMMON_H__

#include "TBDRInput.h"
#include "ShaderCommon.h"

struct Surface
{
	float3	worldPos;
	float	depth;

	float3	albedo;
	float	sunOcclusion;

	float3	normal;
	float	roughness;

	float3	specular;
	float	metallic;
};

void ParseGBufferSurface(out Surface outSurface, uint2 globalIdx, uint sampleIdx)
{
#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( globalIdx, sampleIdx );
#else // non-MSAA
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( uint3(globalIdx, 0) );
#endif

	float3 normal = normal_roughness.xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	outSurface.normal		= normal;
	outSurface.roughness	= normal_roughness.w;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	float depth = g_tDepth.Load( globalIdx, sampleIdx ).x;
#else
	float depth = g_tDepth.Load( uint3(globalIdx, 0) ).x;
#endif

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), tbrParam_invViewProjViewportMat );
	worldPosition /= worldPosition.w;

	outSurface.depth	= depth;
	outSurface.worldPos	= worldPosition.xyz;

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 albedo_sunOcclusion = g_tGBufferAlbedo_sunOcclusion.Load( globalIdx, sampleIdx );
#else
	float4 albedo_sunOcclusion = g_tGBufferAlbedo_sunOcclusion.Load( uint3(globalIdx, 0) );
#endif

	outSurface.albedo		= albedo_sunOcclusion.rgb;
	outSurface.sunOcclusion = albedo_sunOcclusion.a;

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 specular_metallic = g_tGBufferSpecular_metallic.Load( globalIdx, sampleIdx );
#else
	float4 specular_metallic = g_tGBufferSpecular_metallic.Load( uint3(globalIdx, 0) );
#endif

	outSurface.specular	= specular_metallic.rgb;
	outSurface.metallic	= specular_metallic.a;
}

#endif