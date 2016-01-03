//EMPTY_META_DATA

#ifndef __SOC_SHADOW_H__
#define __SOC_SHADOW_H__

#include "ShaderCommon.h"

#define SHADOW_KERNEL_LEVEL		4
#define SHADOW_KERNEL_WIDTH		2 * SHADOW_KERNEL_LEVEL + 1

SamplerComparisonState shadowSamplerCmpState	:	register( s2 );
SamplerState shadowSamplerState					:	register( s3 );

float Shadowing(Texture2D<float> atlas, float2 uv, float depth)
{
	float shadow = 0.0f;
	[unroll] for(int i=-SHADOW_KERNEL_LEVEL; i<=SHADOW_KERNEL_LEVEL; ++i)
	{
		[unroll] for(int j=-SHADOW_KERNEL_LEVEL; j<=SHADOW_KERNEL_LEVEL; ++j)
		{
			shadow += (atlas.SampleCmpLevelZero(shadowSamplerCmpState, uv, depth, int2(i, j)).x);
		}
	}

	shadow /= (float)(SHADOW_KERNEL_WIDTH * SHADOW_KERNEL_WIDTH);
	return shadow;
}

float ChebyshevUpperBound(float2 moments, float t)
{
	float p = (t <= moments.x);

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.00002f);

	float d = t - moments.x;
	float pMax = variance / (variance + d * d);//smoothStep(0.2f, 1.0f, variance / (variance + d * d));

	return max(p, pMax);
}

float2 RecombinePrecision(float4 value)
{
	float invFactor = 1.0f / 256.0f;
	return (value.zw * invFactor + value.xy);
}

float VarianceShadow(Texture2D<float4> momentShadowMapAtlas, float2 uv, float depth)
{
	float shadow = 0.0f;
	float2 moment = float2(0.0f, 0.0f);

	[unroll] for(int i=-SHADOW_KERNEL_LEVEL; i<=SHADOW_KERNEL_LEVEL; ++i)
	{
		[unroll] for(int j=-SHADOW_KERNEL_LEVEL; j<=SHADOW_KERNEL_LEVEL; ++j)
		{
			moment = RecombinePrecision(momentShadowMapAtlas.SampleLevel(shadowSamplerState, uv, 0, int2(i, j)));
			shadow += ChebyshevUpperBound(moment, depth);
		}
	}

	shadow /= (float)(SHADOW_KERNEL_WIDTH * SHADOW_KERNEL_WIDTH);
	return shadow;
}

float3 RenderSpotLightShadow(uint lightIndex, float3 vertexWorldPos, float shadowDistanceTerm)
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
#if defined(USE_SHADOW_INVERTED_DEPTH) && !defined(USE_VSM)
		+
#else
		-
#endif
		lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * bias;

#if defined(USE_VSM)
	#if defined(USE_SHADOW_INVERTED_DEPTH)
		float shadow = saturate( VarianceShadow(g_inputSpotLightMomentShadowMapAtlas, shadowUV.xy, depth) );
	#else
		float shadow = saturate( VarianceShadow(g_inputSpotLightShadowMapAtlas, g_inputSpotLightMomentShadowMapAtlas, shadowUV.xy, depth) );
	#endif
#else
	float shadow = saturate( Shadowing(g_inputSpotLightShadowMapAtlas, shadowUV.xy, depth) );
#endif

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
#if defined(USE_SHADOW_INVERTED_DEPTH) && !defined(USE_VSM)
		+
#else
		-
#endif
		bias;

#if defined(USE_VSM)
	#if defined(USE_SHADOW_INVERTED_DEPTH)
		float shadow = saturate( VarianceShadow(g_inputDirectionalLightMomentShadowMapAtlas, shadowUV.xy, depth) );		
	#else
		float shadow = saturate( VarianceShadow(g_inputDirectionalLightShadowMapAtlas, g_inputDirectionalLightMomentShadowMapAtlas, shadowUV.xy, depth) );		
	#endif
#else
	float shadow = saturate( Shadowing(g_inputDirectionalLightShadowMapAtlas, shadowUV.xy, depth) );
#endif
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
#if defined(USE_SHADOW_INVERTED_DEPTH) && !defined(USE_VSM)
		+
#else
		-
#endif
		lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * bias;

#if defined(USE_VSM)
	#if defined(USE_SHADOW_INVERTED_DEPTH)
		float shadow = saturate( VarianceShadow(g_inputPointLightMomentShadowMapAtlas, shadowUV.xy, depth) );
	#else
		float shadow = saturate( VarianceShadow(g_inputPointLightShadowMapAtlas, g_inputPointLightMomentShadowMapAtlas, shadowUV.xy, depth) );
	#endif
#else
	float shadow = saturate( Shadowing(g_inputPointLightShadowMapAtlas, shadowUV.xy, depth) );
#endif

	float3 shadowColor = g_inputPointLightShadowColors[lightIndex].rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

	float shadowStrength = g_inputPointLightShadowColors[lightIndex].a;
	return lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
}


#endif