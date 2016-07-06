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
	float	occlusion;

	float3	normal;
	float	roughness;

	float3	specular;
	float	metallic;

	float3	emission;
	float	specularity;

	float2	motion;
	uint	materialFlag;
};

void ParseGBufferSurface(out Surface outSurface, uint2 globalIdx, uint sampleIdx)
{
#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 normal_roughness = GBufferNormal_roughness.Load( globalIdx, sampleIdx );
#else // non-MSAA
	float4 normal_roughness = GBufferNormal_roughness.Load( uint3(globalIdx, 0) );
#endif

	float3 normal = normal_roughness.xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	outSurface.normal		= normal;
	outSurface.roughness	= normal_roughness.w;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	float depth = GBufferDepth.Load( globalIdx, sampleIdx ).x;
#else
	float depth = GBufferDepth.Load( uint3(globalIdx, 0) ).x;
#endif

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), tbrParam_invViewProjViewportMat );
	worldPosition /= worldPosition.w;

	outSurface.depth	= depth;
	outSurface.worldPos	= worldPosition.xyz;

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 albedo_occlusion					= GBufferAlbedo_occlusion.Load( globalIdx, sampleIdx );
	float4 motionXY_metallic_specularity	= GBufferMotionXY_metallic_specularity.Load( globalIdx, sampleIdx );
	float3 emission							= GBufferEmission_materialFlag.Load(globalIdx, sampleIdx).rgb;
#else
	float4 albedo_occlusion					= GBufferAlbedo_occlusion.Load( uint3(globalIdx, 0) );
	float4 motionXY_metallic_specularity	= GBufferMotionXY_metallic_specularity.Load( uint3(globalIdx, 0) );
	float3 emission							= GBufferEmission_materialFlag.Load(uint3(globalIdx, 0)).rgb;
#endif
	uint	matFlag			= uint( GBufferEmission_materialFlag.Load(uint3(globalIdx, 0)).a * 255.0f );
	float	specularity		= motionXY_metallic_specularity.a;
	float3	baseColor		= albedo_occlusion.rgb;
	float	metallic		= motionXY_metallic_specularity.b;

	outSurface.albedo		= baseColor - baseColor * metallic;
	outSurface.occlusion	= albedo_occlusion.a;

	outSurface.specular		= lerp(0.08f * specularity.xxx, baseColor, metallic);
	outSurface.metallic		= metallic;

	outSurface.emission		= emission;
	outSurface.specularity	= specularity;

	outSurface.motion		= (motionXY_metallic_specularity.xy * 2.0f) - float2(1.0f, 1.0f);
	outSurface.materialFlag	= matFlag;
}

#endif