//EMPTY_META_DATA

#ifndef __SOC_DYNAMIC_LIGHTING_H__
#define __SOC_DYNAMIC_LIGHTING_H__

#include "ShaderCommon.h"
#include "BRDF.h"

#define SHADOW_KERNEL_LEVEL		4
#define SHADOW_KERNEL_WIDTH		2 * SHADOW_KERNEL_LEVEL + 1

SamplerComparisonState shadowSamplerCmpState	:	register( s2 );

float Shadowing(Texture2D<float> atlas, float2 uv, float depth)
{
	float shadow = 0.0f;
	for(int i=-SHADOW_KERNEL_LEVEL; i<=SHADOW_KERNEL_LEVEL; ++i)
	{
		for(int j=-SHADOW_KERNEL_LEVEL; j<=SHADOW_KERNEL_LEVEL; ++j)
		{
			shadow += (atlas.SampleCmpLevelZero(shadowSamplerCmpState, uv, depth, int2(i, j)).x);
		}
	}

	shadow /= (float)(SHADOW_KERNEL_WIDTH * SHADOW_KERNEL_WIDTH);
	return shadow;
}

float3 RenderSpotLightShadow(uint lightIndex, float3 vertexWorldPos)
{
	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), g_inputSpotLightShadowParams[lightIndex].viewProjMat );
	shadowUV /= shadowUV.w;

	shadowUV.x = (shadowUV.x / 2.0f) + 0.5f;
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	uint shadowIndex = (uint)g_inputSpotLightShadowParams[lightIndex].index - 1;
	shadowUV.x += shadowIndex;

	uint lightCount = GetNumOfSpotLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	shadowUV.x *= rcp((float)lightCount);//(1.0f / (float)lightCount);

	float bias = (float)g_inputSpotLightShadowParams[lightIndex].bias;
	float depth = shadowUV.z
#if defined(USE_SHADOW_INVERTED_DEPTH)
		+
#else
		-
#endif
		bias;
	float shadow = saturate( Shadowing(g_inputSpotLightShadowMapAtlas, shadowUV.xy, depth) );

	float3 shadowColor = g_inputSpotLightShadowColors[lightIndex].rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

	float shadowStrength = g_inputSpotLightShadowColors[lightIndex].a;
	return lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
}

float3 RenderDirectionalLightShadow(uint lightIndex, float3 vertexWorldPos)
{
	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), g_inputDirectionalLightShadowParams[lightIndex].viewProjMat );
	shadowUV /= shadowUV.w;

	shadowUV.x = (shadowUV.x / 2.0f) + 0.5f;
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	uint shadowIndex = (uint)g_inputDirectionalLightShadowParams[lightIndex].index - 1;
	shadowUV.x += shadowIndex;

	uint lightCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	shadowUV.x *= rcp((float)lightCount);//(1.0f / (float)lightCount);

	float bias = (float)g_inputDirectionalLightShadowParams[lightIndex].bias;
	float depth = shadowUV.z
#if defined(USE_SHADOW_INVERTED_DEPTH)
		+
#else
		-
#endif
		bias;
	float shadow = saturate( Shadowing(g_inputDirectionalLightShadowMapAtlas, shadowUV.xy, depth) );

	float3 shadowColor = g_inputDirectionalLightShadowColors[lightIndex].rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

	float shadowStrength = g_inputDirectionalLightShadowColors[lightIndex].a;
	return lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
}

uint ComputeFaceIndex(float3 dir)
{
	// 뭐, 별건 없고
	// PointLight::ComputeViewProjMatrix에서 사용되는 forwards, ups 이거 인덱스 정하는 함수이다.
	// 대체 뭔 방식으로 계산하는지 궁금하다면, 0, 0, 1이나 1, 0, 0 직접 머리속에 넣어서 계산하면된다.
	// 그럼 PointLight::ComputeViewProjMatrix에서 사용 중인 인덱스와 동일하게 뜬다.

	uint res = 0;

	if( (abs(dir.x) > abs(dir.y)) && (abs(dir.x) > abs(dir.z)) )
		res = dir.x >= 0.0f ? 2 : 3;
	else if( (abs(dir.y) > abs(dir.x)) && (abs(dir.y) > abs(dir.z)) )
		res = dir.y >= 0.0f ? 4 : 5;
	else
		res = dir.z >= 0.0f ? 0 : 1;

	return res;
}

float3 RenderPointLightShadow(uint lightIndex, float3 vertexWorldPos, float3 lightDir, float shadowDistanceTerm)
{
	uint faceIndex = ComputeFaceIndex(-lightDir);

	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), g_inputPointLightShadowParams[lightIndex].viewProjMat[faceIndex] );
	shadowUV /= shadowUV.w;

	shadowUV.x = (shadowUV.x / 2.0f) + 0.5f;
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	shadowUV.xy *= shadowGlobalParam_pointLightTexelOffset.xx;
	shadowUV.xy += (shadowGlobalParam_pointLightUnderscanScale).xx;

	shadowUV.y += (float)faceIndex;
	shadowUV.y *= rcp(6); //1.0f / 6.0f;

	uint shadowIndex = (uint)g_inputPointLightShadowParams[lightIndex].index - 1;
	shadowUV.x += (float)shadowIndex;

	uint lightCount = GetNumOfPointLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	shadowUV.x *= rcp((float)lightCount);//(1.0f / (float)lightCount);

	float bias = (float)g_inputPointLightShadowParams[lightIndex].bias;
	float depth = shadowUV.z
#if defined(USE_SHADOW_INVERTED_DEPTH)
		+
#else
		-
#endif
		lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * bias;

	float shadow = saturate( Shadowing(g_inputPointLightShadowMapAtlas, shadowUV.xy, depth) );

	float3 shadowColor = g_inputPointLightShadowColors[lightIndex].rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

	float shadowStrength = g_inputPointLightShadowColors[lightIndex].a;
	return lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
}

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

		uint shadowIndex = (uint)g_inputDirectionalLightShadowParams[lightIndex].index;
		if(shadowIndex != 0) //isShadow == true
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

		uint shadowIndex = (uint)g_inputPointLightShadowParams[lightIndex].index;
		if(shadowIndex != 0) //isShadow == true
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
	float3 lightDir = float3(spotParam.x, spotParam.y, 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
	lightDir.z = lerp(-lightDir.z, lightDir.z, radiusWithMinusZDirBit >= 0.0f);

	float radius = abs(radiusWithMinusZDirBit);

	float outerCosineConeAngle	= spotParam.z;
	float innerCosineConeAngle	= spotParam.w;

	float3 vtxToLight		= lightPos - vertexWorldPosition;
	float3 vtxToLightDir	= normalize(vtxToLight);

	float distanceOfLightWithVertex = length(vtxToLight);

	float currentCosineConeAngle = dot(-vtxToLightDir, lightDir);
	if( (distanceOfLightWithVertex < (radius * 1.5f)) &&
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

		uint shadowIndex = (uint)g_inputSpotLightShadowParams[lightIndex].index;
		if(shadowIndex != 0) //isShadow == true
		{
			float3 shadowColor = RenderSpotLightShadow(lightIndex, vertexWorldPosition);

			resultDiffuseColor	*= shadowColor;
			resultSpecularColor	*= shadowColor;
		}
#endif
	}
}


#endif
