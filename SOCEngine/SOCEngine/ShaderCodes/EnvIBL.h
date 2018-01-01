#ifndef __SOC_ENV_IBL_H__
#define __SOC_ENV_IBL_H__

#include "GBufferParser.h"
#include "CommonConstBuffer.h"
#include "ShaderCommon.h"
#include "BRDF.h"

#define IBL_ACCUM_MIN_RATE 0.2f
#define IBL_ACCUM_MAX_RATE 1.0f

#define USE_APPROXIMATE

Texture2D<float2> preIntegrateEnvBRDFMap	: register( t29 );
TextureCube	skyCubeMap						: register( t32 );

SamplerState skyCubeMapSampler				: register( s5  );

static const float MaxMipLevel = 7.0f;

float3 GetSkyLightReflection(float3 reflectDir, float roughness, bool isDynamicSkyLight)
{
	float absSpecularMip	= ComputeRoughnessLOD(roughness, MaxMipLevel);
	float3 reflection		= skyCubeMap.SampleLevel(skyCubeMapSampler, reflectDir, absSpecularMip).rgb;

	if(isDynamicSkyLight)
	{
		float3 LowFrequencyReflection = skyCubeMap.SampleLevel(skyCubeMapSampler, reflectDir, MaxMipLevel).rgb;
		float LowFrequencyBrightness = Luminance(LowFrequencyReflection);

		reflection /= max(LowFrequencyBrightness, 0.00001f);
	}

	return reflection;
}

float3 PreFilterEnvMap(float roughness, float3 reflectDir, uniform uint numSample, uint2 random)
{
	float3	filteredColor	= float3(0.0f, 0.0f, 0.0f);
	float	weight			= 0.0f;
		
	for( uint i = 0; i < numSample; i++ )
	{
		float2 e		= Hammersley(i, numSample, random);
		float3 halfVec	= TangentToWorld( ImportanceSampleGGX( e, roughness ).xyz, reflectDir );
		float3 lightDir	= 2 * dot( reflectDir, halfVec ) * halfVec - reflectDir;

		float NdotL		= saturate( dot( reflectDir, lightDir ) );
		if( NdotL > 0 )
		{
			filteredColor += skyCubeMap.SampleLevel( skyCubeMapSampler, lightDir, 0 ).rgb * NdotL;
			weight += NdotL;
		}
	}

	return filteredColor / max( weight, 0.001 );
}

float3 ApproximateSpecularIBL(Texture2D<float2> preIntegrateEnvMap, float3 specularColor,
							float roughness, float3 normal, float3 viewDir, uniform bool usePreIntegrateEnvMap,
							uint2 random)
{
	float NdotV			= abs(dot(normal, viewDir));
	float3 reflectDir	= 2.0f * dot(normal, viewDir) * normal - viewDir;
	float mip			= ComputeRoughnessLOD(roughness, MaxMipLevel);

	float3 prefilteredColor	= float3(0.0f, 0.0f, 0.0f);
	float2 envBRDF			= float2(0.0f, 0.0f);

	roughness	= abs(roughness - 0.0002f);
	NdotV		= abs(NdotV - 0.0002f);
	
	if(usePreIntegrateEnvMap)
	{
		prefilteredColor	= skyCubeMap.SampleLevel(skyCubeMapSampler, reflectDir, mip).rgb;
		envBRDF				= saturate(preIntegrateEnvMap.SampleLevel(skyCubeMapSampler, float2(roughness, NdotV), 0));
	}
	else
	{
		prefilteredColor	= PreFilterEnvMap(roughness, reflectDir, BRDF_SAMPLES, random).rgb;
		envBRDF				= saturate(IntegrateBRDF(roughness, NdotV, BRDF_SAMPLES, random));
	}

	return prefilteredColor * (specularColor * envBRDF.x + envBRDF.y);
}

float3 SpecularIBL(float3 specularColor, float roughness, float3 normal, float3 viewDir, uniform uint numSamples, uint2 random)
{
	float3 specularLighting = 0;

	for( uint i = 0; i < numSamples; i++ )
	{
		float2 e		= Hammersley(i, numSamples, random);
		float3 halfVec	= TangentToWorld( ImportanceSampleGGX( e, roughness ).xyz, normal );
		float3 lightDir	= 2 * dot( viewDir, halfVec ) * halfVec - viewDir;

		float NdotV = saturate( dot( normal,	viewDir	 ) );
		float NdotL = saturate( dot( normal,	lightDir ) );
		float NdotH = saturate( dot( normal,	halfVec	 ) );
		float VdotH = saturate( dot( viewDir,	halfVec	 ) );
		
		if( NdotL > 0 )
		{
			float3 SampleColor = skyCubeMap.SampleLevel( skyCubeMapSampler, lightDir, 0 ).rgb;

			float Vis	= GeometrySmithJointApproximately( NdotV, NdotL, roughness );
			float Fc	= pow( 1.0f - VdotH, 5.0f );
			float3 F	= (1.0f - Fc) * specularColor + Fc;

			// Incident light = SampleColor * NdotL
			// Microfacet specular = D*G*F / (4*NdotL*NdotV) = D*Vis*F
			// pdf = D * NdotH / (4 * VdotH)
			specularLighting += SampleColor * F * ( NdotL * Vis * (4.0f * VdotH / NdotH) );
		}
	}

	return specularLighting / float(numSamples);
}

float3 DiffuseIBL(float3 diffuseColor, float roughness, float3 normal, float3 viewDir, uniform uint numSamples, uint2 random)
{
	normal	= normalize( normal );
	viewDir	= normalize( viewDir );

	float3 diffuseLighting = 0;
	
	float NdotV = saturate( dot( normal, viewDir ) );

	for( uint i = 0; i < numSamples; i++ )
	{
		float2 e		= Hammersley(i, numSamples, random);
		float3 lightDir	= TangentToWorld( CosineSampleHemisphere( e ).xyz, normal );
		float3 halfVec	= normalize(viewDir + lightDir);

		float NdotL = saturate( dot( normal, lightDir ) );
		float NdotH	= saturate( dot( normal, halfVec ) );
		float VdotH	= saturate( dot( viewDir, halfVec ) );

		if( NdotL > 0 )
		{
			float3 SampleColor = skyCubeMap.SampleLevel( skyCubeMapSampler, lightDir, 0 ).rgb;

			float FD90 = ( 0.5 + 2 * VdotH * VdotH ) * roughness;
			//float FD90 = 0.5 + 2 * VdotH * VdotH * roughness;
			float FdV = 1 + (FD90 - 1) * pow( 1 - NdotV, 5 );
			float FdL = 1 + (FD90 - 1) * pow( 1 - NdotL, 5 );

			// lambert = diffuseColor * NdotL / PI
			// pdf = NdotL / PI
			diffuseLighting += SampleColor * diffuseColor * FdV * FdL * ( 1 - 0.3333f * roughness );
		}
	}

	return diffuseLighting / float(numSamples);
}

struct ApproximateIBLParam
{
	float3	diffuseColor;
	float3	specularColor;

	float	roughness;
	float3	normal;
	float3	viewDir;
};

float3 ApproximateIBL(Texture2D<float2> preIntegrateEnvMap, ApproximateIBLParam param)
{
	float3 diffuse = float3(0.0f, 0.0f, 0.0f);
	{
		float3 diffsueLookUp = skyCubeMap.SampleLevel(skyCubeMapSampler, param.normal, MaxMipLevel).rgb;			
		diffuse = param.diffuseColor * diffsueLookUp;
	}

	float3 specular = ApproximateSpecularIBL(preIntegrateEnvMap, param.specularColor, param.roughness, param.normal, param.viewDir, true, uint2(0, 0));
	return diffuse + specular;
}

void IBL(out float3 outDiffuse, out float3 outSpecular, Surface surface)
{
	float3 normal		= surface.normal;
	float3 viewDir		= normalize( camera_worldPos - surface.worldPos );
	float3 reflectDir	= reflect(-viewDir, normal);

	float3 diffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 specular	= float3(0.0f, 0.0f, 0.0f);

	uint2 random = uint2(0, 0);
	// Diffuse
	{
#ifdef USE_APPROXIMATE
		float3 diffsueLookUp = skyCubeMap.SampleLevel(skyCubeMapSampler, normal, MaxMipLevel).rgb;
		diffuse = surface.albedo * diffsueLookUp;
#else
		diffuse = DiffuseIBL(surface.albedo, surface.roughness, surface.normal, viewDir, BRDF_SAMPLES, random);
#endif
	}

	// Specular
	{
#ifdef USE_APPROXIMATE
		specular = ApproximateSpecularIBL(preIntegrateEnvBRDFMap, surface.specular,
			surface.roughness, surface.normal, viewDir,
			true, random);
#else
		specular = SpecularIBL(surface.specular, surface.roughness, surface.normal, viewDir, BRDF_SAMPLES, random);
#endif
	}

	outDiffuse	= diffuse * float(surface.depth > 0.0f);
	outSpecular	= specular * float(surface.depth > 0.0f);
}

#endif