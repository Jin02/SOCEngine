//EMPTY_META_DATA

#ifndef __SOC_DYNAMIC_LIGHTING_H__
#define __SOC_DYNAMIC_LIGHTING_H__

#include "ShaderCommon.h"
#include "BRDF.h"

void RenderDirectionalLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, 
#endif
					in LightingParams lightingParams)
{
#if defined(RENDER_TRANSPARENCY)
	resultFrontFaceDiffuseColor = resultFrontFaceSpecularColor = float3(0, 0, 0);
	resultBackFaceDiffuseColor = resultBackFaceSpecularColor = float3(0, 0, 0);
#else
	resultDiffuseColor	= float3(0, 0, 0);
	resultSpecularColor	= float3(0, 0, 0);
#endif

	float4	lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition = lightCenterWithDirZ.xyz;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].xyz;

		float2	lightParam			= g_inputDirectionalLightParamBuffer[lightingParams.lightIndex];
		commonParams.lightDir		= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		float intensity = g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].a * 10.0f;
#if defined(RENDER_TRANSPARENCY)
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, lightingParams, commonParams);
		resultFrontFaceDiffuseColor		*= intensity;
		resultFrontFaceSpecularColor	*= intensity;

		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, lightingParams, commonParams);
		resultBackFaceDiffuseColor		*= intensity;
		resultBackFaceSpecularColor		*= intensity;
#else
		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);
		resultDiffuseColor				*= intensity;
		resultSpecularColor				*= intensity;
#endif
	}	
}

void RenderPointLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, 
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
#if defined(RENDER_TRANSPARENCY)
	resultFrontFaceDiffuseColor	= resultFrontFaceSpecularColor	= float3(0, 0, 0);
	resultBackFaceDiffuseColor	= resultBackFaceSpecularColor	= float3(0, 0, 0);
#else
	resultDiffuseColor	= float3(0, 0, 0);
	resultSpecularColor	= float3(0, 0, 0);
#endif

	float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightingParams.lightIndex];

	float3 lightCenterWorldPosition	= lightCenterWithRadius.xyz;
	float lightRadius				= lightCenterWithRadius.w;

	float3 lightDir					= lightCenterWorldPosition - vertexWorldPosition;
	float distanceOfLightWithVertex = length(lightDir);
	lightDir = normalize(lightDir);

	if( distanceOfLightWithVertex < lightRadius )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputPointLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightDir		= lightDir;

		float lumen = g_inputPointLightColorBuffer[lightingParams.lightIndex].w * 12750.0f; //maximum lumen is 12,750f
		float attenuation = 1.0f / (distanceOfLightWithVertex * distanceOfLightWithVertex) * lumen;

#if defined(RENDER_TRANSPARENCY)
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, lightingParams, commonParams);
		resultFrontFaceDiffuseColor		*= attenuation;
		resultFrontFaceSpecularColor	*= attenuation;

		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, lightingParams, commonParams);		
		resultBackFaceDiffuseColor		*= attenuation;
		resultBackFaceSpecularColor		*= attenuation;
#else
		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);

		resultDiffuseColor	*= attenuation;
		resultSpecularColor	*= attenuation;
#endif
	}
}

void RenderSpotLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, 
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
#if defined(RENDER_TRANSPARENCY)
	resultFrontFaceDiffuseColor = resultFrontFaceSpecularColor = float3(0, 0, 0);
	resultBackFaceDiffuseColor = resultBackFaceSpecularColor = float3(0, 0, 0);
#else
	resultDiffuseColor	= float3(0, 0, 0);
	resultSpecularColor	= float3(0, 0, 0);
#endif

	uint lightIdx = lightingParams.lightIndex;

	float3 lightPos	= g_inputSpotLightTransformBuffer[lightIdx].xyz;
	float radius	= g_inputSpotLightTransformBuffer[lightIdx].w;

	float4 spotParam = g_inputSpotLightParamBuffer[lightIdx];
	float3 lightDir = float3(spotParam.x, spotParam.y, 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
	lightDir.z = lerp(-lightDir.z, lightDir.z, radius >= 0.0f);

	radius = abs(radius);

	float outerCosineConeAngle	= g_inputSpotLightParamBuffer[lightIdx].z;
	float innerCosineConeAngle	= g_inputSpotLightParamBuffer[lightIdx].w;

	float3 vtxToLight		= lightPos - vertexWorldPosition;
	float3 vtxToLightDir	= normalize(vtxToLight);

	float distanceOfLightWithVertex = length(vtxToLight);

	float currentCosineConeAngle = dot(-vtxToLightDir, lightDir);
	if( (distanceOfLightWithVertex < (radius * 1.5f)) &&
		(outerCosineConeAngle < currentCosineConeAngle) )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputSpotLightColorBuffer[lightIdx].xyz;
		commonParams.lightDir		= vtxToLightDir;

		float innerOuterAttenuation = saturate( (currentCosineConeAngle - outerCosineConeAngle) / (innerCosineConeAngle - outerCosineConeAngle));
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
		innerOuterAttenuation = lerp(innerOuterAttenuation, 1, innerCosineConeAngle < currentCosineConeAngle);

		float lumen = g_inputSpotLightColorBuffer[lightIdx].w * 12750.0f; //maximum lumen is 12750.0f

		float plAttenuation = 1.0f / (distanceOfLightWithVertex * distanceOfLightWithVertex);
		float totalAttenTerm = lumen * plAttenuation * innerOuterAttenuation;

#if defined(RENDER_TRANSPARENCY)
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, lightingParams, commonParams);
		resultFrontFaceDiffuseColor		*= totalAttenTerm;
		resultFrontFaceSpecularColor	*= totalAttenTerm;

		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, lightingParams, commonParams);		
		resultBackFaceDiffuseColor		*= totalAttenTerm;
		resultBackFaceSpecularColor		*= totalAttenTerm;
#else
		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);

		resultDiffuseColor	*= totalAttenTerm;
		resultSpecularColor	*= totalAttenTerm;
#endif
	}
}


#endif