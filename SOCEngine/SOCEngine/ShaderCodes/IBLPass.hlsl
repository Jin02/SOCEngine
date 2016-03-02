//NOT_CREATE_META_DATA

#include "FullScreenShader.h"
#include "GBufferParser.h"
#include "EnvIBL.h"

Texture2D<float2> preIntegrateEnvBRDFMap : register( t29 );

float ComputeCubeMapMipFromRoughness(float roughness, float mipCount)
{
	float level = 3.0f - 1.15f * log2(roughness);
	return max(mipCount - 1.0f - level, 0);
}

float4 PS( PS_INPUT input ) : SV_Target
{
	uint2 screenPt		= input.position.xy;

	Surface surface;
#if (MSAA_SAMPLES_COUNT > 1)
	ParseGBufferSurface(surface, screenPt, input.sampleIdx);
#else
	ParseGBufferSurface(surface, screenPt, 0);
#endif

	float3 normal	= surface.normal;
	float3 viewDir	= normalize( tbrParam_cameraWorldPosition - surface.worldPos );

	float3 diffuse	= 0;
	float3 specular	= 0;

	// Diffuse
	{
#ifndef USE_IMPORTANCE_DIFFUSE
		float absDiffuseMip = skyMapInfoParam_maxMipCount;
		float3 diffsueLookUp = skyCubeMap.SampleLevel(skyCubeMapSampler, normal, absDiffuseMip).rgb;
		
		diffuse = surface.albedo * diffsueLookUp;
#else
		diffuse = DiffuseIBL(surface.albedo, surface.roughness, surface.normal, viewDir, BRDF_SAMPLES);
#endif
	}

	// Specular
	{
#ifdef USE_APPROXIMATE_SPECULAR_IBL
		specular = ApproximateSpecularIBL(preIntegrateEnvBRDFMap, specularColor,
											surface.roughness, surface.normal, viewDir, true);
#else
		specular = SpecularIBL(specularColor, surface.roughness, surface.normal, viewDir, BRDF_SAMPLES);
#endif
	}

	// float ao = surface.ao * screenSpace.ao;

	float3 result = (diffuse + specular);
	return result;
}