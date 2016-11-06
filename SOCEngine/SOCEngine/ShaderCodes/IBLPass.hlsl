//EMPTY_META_DATA

#define USE_IMPORTANCE_DIFFUSE_IBL
#define USE_APPROXIMATE_SPECULAR_IBL

#include "FullScreenShader.h"
#include "GBufferParser.h"
#include "CommonConstBuffer.h"
#include "EnvIBL.h"

Texture2D<float2> preIntegrateEnvBRDFMap	: register( t29 );
Texture2D<float4> illuminationMap		: register( t30 );

SamplerState linerSamplerState			: register( s0 );

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

	float3 normal		= surface.normal;
	float3 viewDir		= normalize( camera_worldPos - surface.worldPos );
	float3 reflectDir	= reflect(-viewDir, normal);

	float3 diffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 specular	= float3(0.0f, 0.0f, 0.0f);

	// Diffuse
	{
#ifdef USE_IMPORTANCE_DIFFUSE_IBL
		diffuse = DiffuseIBL(surface.albedo, surface.roughness, surface.normal, viewDir, BRDF_SAMPLES);
#else
		float absDiffuseMip = skyMapInfoParam_maxMipCount;
		float3 diffsueLookUp = skyCubeMap.SampleLevel(skyCubeMapSampler, normal, absDiffuseMip).rgb;
		
		diffuse = surface.albedo * diffsueLookUp;
#endif
	}

	// Specular
	{
#ifdef USE_APPROXIMATE_SPECULAR_IBL
		specular = ApproximateSpecularIBL(preIntegrateEnvBRDFMap, surface.specular,
											surface.roughness, surface.normal, viewDir,
											true);
#else
		specular = SpecularIBL(surface.specular, surface.roughness, surface.normal, viewDir, BRDF_SAMPLES);
#endif
	}

	// float ao = surface.ao * screenSpace.ao;
	bool existDepth = surface.depth > 0.0f;

	float4 illumination = illuminationMap.Sample(linerSamplerState, input.uv);
	float3 result = illumination.rgb + (diffuse + specular) * float(existDepth);
	return float4(result, 1.0f);
}
