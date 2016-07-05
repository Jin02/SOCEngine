#ifndef __SOC_ENV_IBL_H__
#define __SOC_ENV_IBL_H__

#define NOT_USE_BRDF_LIGHTING

#include "../GlobalDefine.h"
#include "ShaderCommon.h"
#include "BRDF.h"

SamplerState	AmbientCubeMapSampler	: register( s4  );
TextureCube	AmbientCubeMap		: register( t32 );

cbuffer SkyMapInfoParam			: register( b7 )
{
	float	skyMapInfoParam_maxMipCount;
	uint	skyMapInfoParam_isSkyLightOn;
	uint	skyMapInfoParam_isDynamicSkyLight;
	float	skyMapInfoParam_blendFraction;
};

float3 GetSkyLightReflection(float3 reflectDir, float roughness, bool isDynamicSkyLight)
{
	const float maxMipCount = skyMapInfoParam_maxMipCount;

	float absSpecularMip	= ComputeRoughnessLOD(roughness, maxMipCount);
	float3 reflection	= AmbientCubeMap.SampleLevel(AmbientCubeMapSampler, reflectDir, absSpecularMip).rgb;

	if(isDynamicSkyLight)
	{
		float3 LowFrequencyReflection	= AmbientCubeMap.SampleLevel(AmbientCubeMapSampler, reflectDir, maxMipCount).rgb;
		float LowFrequencyBrightness	= Luminance(LowFrequencyReflection);

		reflection /= max(LowFrequencyBrightness, 0.00001f);
	}

	return reflection;
}

float3 PreFilterEnvMap(float roughness, float3 reflectDir, uniform uint numSample)
{
	float3	filteredColor	= float3(0.0f, 0.0f, 0.0f);
	float	weight		= 0.0f;
		
	for( uint i = 0; i < numSample; i++ )
	{
		float2 e	= Hammersley( i, numSample );
		float3 halfVec	= TangentToWorld( ImportanceSampleGGX( e, roughness ).xyz, reflectDir );
		float3 lightDir	= 2 * dot( reflectDir, halfVec ) * halfVec - reflectDir;

		float NdotL	= saturate( dot( reflectDir, lightDir ) );
		if( NdotL > 0 )
		{
			filteredColor += AmbientCubeMap.SampleLevel( AmbientCubeMapSampler, lightDir, 0 ).rgb * NdotL;
			weight += NdotL;
		}
	}

	return filteredColor / max( weight, 0.001 );
}

float3 ApproximateSpecularIBL(Texture2D<float2> PreIntegrateEnvMap, float3 specularColor, float roughness, float3 normal, float3 viewDir, uniform bool usePreIntegrateEnvMap)
{
	float NdotV			= abs(dot(normal, viewDir));
	float3 reflectDir		= 2.0f * dot(normal, viewDir) * normal - viewDir;
	float mip			= ComputeRoughnessLOD(roughness, skyMapInfoParam_maxMipCount);

	float3 prefilteredColor		= float3(0.0f, 0.0f, 0.0f);
	float2 envBRDF			= float2(0.0f, 0.0f);

//	roughness	= saturate(roughness - 0.0002f);
//	NdotV		= saturate(NdotV - 0.0002f);
	
	if(usePreIntegrateEnvMap)
	{
		prefilteredColor	= AmbientCubeMap.SampleLevel(AmbientCubeMapSampler, reflectDir, mip).rgb;
		envBRDF			= saturate(PreIntegrateEnvMap.SampleLevel(AmbientCubeMapSampler, float2(roughness, NdotV), 0));
	}
	else
	{
		prefilteredColor	= PreFilterEnvMap(roughness, reflectDir, BRDF_SAMPLES).rgb;
		envBRDF			= saturate(IntegrateBRDF(roughness, NdotV, BRDF_SAMPLES));
	}

	return prefilteredColor * (specularColor * envBRDF.x + envBRDF.y);
}

float3 SpecularIBL(float3 specularColor, float roughness, float3 normal, float3 viewDir, uniform uint numSamples)
{
	float3 specularLighting = 0;

	for( uint i = 0; i < numSamples; i++ )
	{
		float2 e		= Hammersley( i, numSamples );
		float3 halfVec		= TangentToWorld( ImportanceSampleGGX( e, roughness ).xyz, normal );
		float3 lightDir		= 2 * dot( viewDir, halfVec ) * halfVec - viewDir;

		float NdotV = saturate( dot( normal,	viewDir	 ) );
		float NdotL = saturate( dot( normal,	lightDir ) );
		float NdotH = saturate( dot( normal,	halfVec	 ) );
		float VdotH = saturate( dot( viewDir,	halfVec	 ) );
		
		if( NdotL > 0 )
		{
			float3 SampleColor	= AmbientCubeMap.SampleLevel( AmbientCubeMapSampler, lightDir, 0 ).rgb;

			float Vis		= GeometrySmithJointApproximately( NdotV, NdotL, roughness );
			float Fc		= pow( 1.0f - VdotH, 5.0f );
			float3 F		= (1.0f - Fc) * specularColor + Fc;

			// Incident light = SampleColor * NdotL
			// Microfacet specular = D*G*F / (4*NdotL*NdotV) = D*Vis*F
			// pdf = D * NdotH / (4 * VdotH)
			specularLighting += SampleColor * F * ( NdotL * Vis * (4.0f * VdotH / NdotH) );
		}
	}

	return specularLighting / float(numSamples);
}

float3 DiffuseIBL(float3 diffuseColor, float roughness, float3 normal, float3 viewDir, uniform uint numSamples)
{
	normal	= normalize( normal );
	viewDir	= normalize( viewDir );

	float3 diffuseLighting = 0;
	
	float NdotV = saturate( dot( normal, viewDir ) );

	for( uint i = 0; i < numSamples; i++ )
	{
		float2 e	= Hammersley( i, numSamples );
		float3 lightDir	= TangentToWorld( CosineSampleHemisphere( e ).xyz, normal );
		float3 halfVec	= normalize(viewDir + lightDir);

		float NdotL 	= saturate( dot( normal, lightDir ) );
		float NdotH	= saturate( dot( normal, halfVec ) );
		float VdotH	= saturate( dot( viewDir, halfVec ) );

		if( NdotL > 0 )
		{
			float3 SampleColor = AmbientCubeMap.SampleLevel( AmbientCubeMapSampler, lightDir, 0 ).rgb;

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

float3 ApproximateIBL(Texture2D<float2> PreIntegrateEnvMap, ApproximateIBLParam param)
{
	float3 diffuse = float3(0.0f, 0.0f, 0.0f);
	{
		float absDiffuseMip = skyMapInfoParam_maxMipCount;
		float3 diffsueLookUp = AmbientCubeMap.SampleLevel(AmbientCubeMapSampler, param.normal, absDiffuseMip).rgb;
			
		diffuse = param.diffuseColor * diffsueLookUp;
	}

	float3 specular = ApproximateSpecularIBL(PreIntegrateEnvMap, param.specularColor, param.roughness, param.normal, param.viewDir, true);
	return diffuse + specular;
}

#endif
