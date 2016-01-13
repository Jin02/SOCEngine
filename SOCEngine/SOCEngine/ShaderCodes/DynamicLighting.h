//EMPTY_META_DATA

#ifndef __SOC_DYNAMIC_LIGHTING_H__
#define __SOC_DYNAMIC_LIGHTING_H__

#include "Shadow.h"
#include "BRDF.h"

void RenderDirectionalLight(
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

	uint lightIndex = lightingParams.lightIndex;

	float4	lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightIndex];
	float3	lightCenterWorldPosition = lightCenterWithDirZ.xyz;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= g_inputDirectionalLightColorBuffer[lightIndex].xyz;

		float2	lightParam			= g_inputDirectionalLightParamBuffer[lightIndex];
		commonParams.lightDir		= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		float intensity = g_inputDirectionalLightColorBuffer[lightIndex].a * 10.0f;
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

		uint shadowIndex = g_inputDirectionalLightShadowIndex[lightIndex];
		if(shadowIndex != -1) //isShadow == true
		{
			float3 shadowColor = RenderDirectionalLightShadow(lightIndex, vertexWorldPosition);

			resultDiffuseColor				*= shadowColor;
			resultSpecularColor				*= shadowColor;
		}
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
	uint lightIndex = lightingParams.lightIndex;
	float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightIndex];

	float3 lightCenterWorldPosition	= lightCenterWithRadius.xyz;
	float lightRadius				= lightCenterWithRadius.w;

	float3 lightDir					= lightCenterWorldPosition - vertexWorldPosition;
	float distanceOfLightWithVertex = length(lightDir);
	lightDir = normalize(lightDir);

	if( distanceOfLightWithVertex < lightRadius )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputPointLightColorBuffer[lightIndex].xyz;
		commonParams.lightDir		= lightDir;

		float lumen = g_inputPointLightColorBuffer[lightIndex].w * 12750.0f; //maximum lumen is 12,750f
		float attenuation = lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);

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

		uint shadowIndex = g_inputPointLightShadowIndex[lightIndex];
		if(shadowIndex != -1) //isShadow == true
		{
			float3 shadowColor = RenderPointLightShadow(lightIndex, vertexWorldPosition, lightDir, distanceOfLightWithVertex / lightRadius);

			resultDiffuseColor	*= shadowColor;
			resultSpecularColor	*= shadowColor;
		}
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

	uint lightIndex = lightingParams.lightIndex;

	float4 lightCenterPosWithRadius = g_inputSpotLightTransformBuffer[lightIndex];
	float3 lightPos	= lightCenterPosWithRadius.xyz;
	float radiusWithMinusZDirBit = lightCenterPosWithRadius.w;

	float4 spotParam = g_inputSpotLightParamBuffer[lightIndex];
	float3 lightDir = -float3(spotParam.x, spotParam.y, 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
	lightDir.z = (radiusWithMinusZDirBit < 0.0f) ? -lightDir.z : lightDir.z;

	float radius = abs(radiusWithMinusZDirBit);

	float outerCosineConeAngle	= spotParam.z;
	float innerCosineConeAngle	= spotParam.w;

	float3 vtxToLight		= lightPos - vertexWorldPosition;
	float3 vtxToLightDir	= normalize(vtxToLight);

	float distanceOfLightWithVertex = length(vtxToLight);

	float currentCosineConeAngle = dot(-vtxToLightDir, lightDir);
	if( (distanceOfLightWithVertex < (radius * 2.0f)) &&
		(outerCosineConeAngle < currentCosineConeAngle) )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputSpotLightColorBuffer[lightIndex].xyz;
		commonParams.lightDir		= vtxToLightDir;

		float innerOuterAttenuation = saturate( (currentCosineConeAngle - outerCosineConeAngle) / (innerCosineConeAngle - outerCosineConeAngle));
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
		innerOuterAttenuation = lerp(innerOuterAttenuation, 1, innerCosineConeAngle < currentCosineConeAngle);

		float lumen = g_inputSpotLightColorBuffer[lightIndex].w * 12750.0f; //maximum lumen is 12750.0f

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

		uint shadowIndex = g_inputPointLightShadowIndex[lightIndex];
		if(shadowIndex != -1) //isShadow == true
		{
			float3 shadowColor = RenderSpotLightShadow(lightIndex, vertexWorldPosition, distanceOfLightWithVertex / radius);

			resultDiffuseColor	*= shadowColor;
			resultSpecularColor	*= shadowColor;
		}
#endif
	}
}


#endif
