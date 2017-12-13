//EMPTY_META_DATA

#ifndef __SOC_REFLECTION_PROBE_COMMON_H__
#define __SOC_REFLECTION_PROBE_COMMON_H__

#define TRANSPARENCY_BACK_FACE_WEIGHT 0.5f

#include "PhysicallyBased_Common.h"
#include "DynamicLighting.h"
#include "EnvIBL.h"

cbuffer RPInfo : register( b5 )
{
	matrix	rpInfo_viewProjs[6];

	uint	rpInfo_packedNumOfLights;
	float3	rpInfo_camWorldPos;

	float	rpInfo_range;
	float	rpInfo_near;
	float2	dummy;
};

SamplerState		defaultSampler			: register( s0 );
Texture2D<float2>	preIntegrateEnvBRDFMap	: register( t29 );

float3 IBLPass(float2 uv, float3 worldPos, float3 normal)
{
	ApproximateIBLParam param;
	{
		float3 baseColor	= GetDiffuse(defaultSampler, uv).rgb;

		float metallic		= GetMetallic(defaultSampler, uv);
		float specularity	= GetMaterialSpecularity();

		GetDiffuseSpecularColor(param.diffuseColor, param.specularColor, baseColor, specularity, metallic);

		param.normal	= normal;
		param.viewDir	= rpInfo_camWorldPos  - worldPos;
		param.roughness = GetRoughness(defaultSampler, uv);
	}
#if defined(RENDER_TRANSPARENCY)
	float3 frontFaceIBL	= ApproximateIBL(preIntegrateEnvBRDFMap, param);

	param.normal = -normal;
	float3 backFaceIBL	= ApproximateIBL(preIntegrateEnvBRDFMap, param) * TRANSPARENCY_BACK_FACE_WEIGHT;
	float3 result = frontFaceIBL + backFaceIBL;
#else
	float3 result = ApproximateIBL(preIntegrateEnvBRDFMap, param);
#endif

	return result;
}

float4 ReflectionProbeLighting(float3 normal, float3 worldPos, float2 uv)
{
	float3 ibl		= IBLPass(uv, worldPos, normal);
	return float4(GetDiffuse(defaultSampler, uv).rgb + ibl, 1.0f);
}

#endif
