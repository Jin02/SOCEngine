//EMPTY_META_DATA

#include "PhysicallyBased_Common.h"
#include "../Core/BRDF.h"
#include "../Core/LightCulling.h"

#define TRANSPARENCY_BACK_FACE_WEIGHT 0.5f

Buffer<uint> g_perLightIndicesInTile	: register( t13 );
SamplerState transparencyDefaultSampler	: register( s0 );

void RenderDirectionalLightTwoSided(
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
					in LightingParams lightingParam)
{
	float4	lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition = lightCenterWithDirZ.xyz;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].w;

		float4	lightParam			= g_inputDirectionalLightParamBuffer[lightingParams.lightIndex];
		commonParams.lightDir		= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, commonParams, lightingParams);

		lightingParam.normal = -lightingParam.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, commonParams, lightingParams);
	}	
}

void RenderPointLightTwoSided(
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
					in LightingParams lightingParam, float3 vertexWorldPosition)
{
	float3 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightingParams.lightIndex];
	float3 lightCenterWorldPosition	= lightCenterWithRadius.xyz;

	float3 lightDir					= lightCenterWorldPosition - vertexWorldPosition;
	float distanceOfLightAndVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float lightRadius				= lightCenterWithRadius.w;

	if( distanceOfLightAndVertex < lightRadius )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputPointLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputPointLightColorBuffer[lightingParams.lightIndex].w;
		commonParams.lightDir		= lightDir;

		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, commonParams, lightingParams);

		float x = distanceOfLightAndVertex / lightRadius;
		float k = 100.0f - commonParams.lightIntensity; //testing
		float falloff = -(1.0f / k) * (1.0f - (k + 1) / (1.0f + k * x * x) );

		resultFrontFaceDiffuseColor		*= falloff;
		resultFrontFaceSpecularColor	*= falloff;

		lightingParam.normal = -lightingParam.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, commonParams, lightingParams);		

		resultBackFaceDiffuseColor		*= falloff;
		resultBackFaceSpecularColor		*= falloff;
	}
}

void RenderSpotLightTwoSided(
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
					in LightingParams lightingParam, float3 vertexWorldPosition)
{
	float4 spotLightParam	= g_inputSpotLightParamBuffer[lightingParams.lightIndex];
	float3 spotLightDir;
	{
		spotLightDir.xy		= spotLightParam.xy;
		spotLightDir.z		= sqrt(1.0f - spotLightDir.x*spotLightDir.x - spotLightDir.y*spotLightDir.y);	

		bool isDirZMinus	= spotLightParam.w < 0;
		spotLightDir.z		= spotLightDir.z * (1 - (2 * (uint)isDirZMinus));
	}

	float3	lightCenterWithRadius		= g_inputSpotLightTransformBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition	= lightCenterWithRadius.xyz;
	float	lightRadius					= lightCenterWithRadius.w;

	float3	lightRealWorldPosition		= lightCenterWorldPosition - (spotLightDir * lightRadius);
	float3	lightDir					= lightRealWorldPosition - vertexWorldPosition;
	float	distanceOfLightAndVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float	lightCosineConeAngle		= spotLightParam.z;
	float	currentCosineConeAngle		= dot(-lightDir, spotLightDir);


	if( (distanceOfLightAndVertex < lightRadius) && 
		(lightCosineConeAngle < currentCosineConeAngle) )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputSpotLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputSpotLightColorBuffer[lightingParams.lightIndex].w;
		commonParams.lightDir		= lightDir;

		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, commonParams, lightingParams);

		float falloffCoef = abs(spotLightParam.w);
		float x = distanceOfLightAndVertex / lightRadius;
		float falloff = -(1.0f / falloffCoef) * (1.0f - (falloffCoef + 1) / (1.0f + falloffCoef * x * x) );

		resultFrontFaceDiffuseColor		*= falloff;
		resultFrontFaceSpecularColor	*= falloff;

		lightingParam.normal = -lightingParam.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, commonParams, lightingParams);		

		resultBackFaceDiffuseColor		*= falloff;
		resultBackFaceSpecularColor		*= falloff;
	}
}