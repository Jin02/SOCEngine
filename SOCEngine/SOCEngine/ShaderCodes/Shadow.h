//EMPTY_META_DATA

#ifndef __SOC_SHADOW_H__
#define __SOC_SHADOW_H__

#include "TBDRInput.h"
#include "ShaderCommon.h"

#define SHADOW_KERNEL_LEVEL				4
#define SHADOW_KERNEL_WIDTH				2 * SHADOW_KERNEL_LEVEL + 1
#define SHADOW_PARAM_FLAG_USE_VSM		1

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
	float pMax = smoothStep(0.2f, 1.0f, variance / (variance + d * d));

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

float4 RenderSpotLightShadow(uint lightIndex, float3 vertexWorldPos, float shadowDistanceTerm)
{
	uint shadowIndex = GetShadowIndex(SpotLightOptionalParamIndex[lightIndex]);

	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), SpotLightShadowViewProjMatrix[shadowIndex].mat);
	shadowUV /= shadowUV.w;

	uint lightCount = GetNumOfSpotLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	shadowUV.x = ((shadowUV.x / 2.0f) + 0.5f + float(shadowIndex)) * rcp((float)lightCount);
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	ParsedShadowParam shadowParam;
	ParseShadowParam(shadowParam, SpotLightShadowParams[shadowIndex]);

	float bias	= lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * shadowParam.bias;
	float depth	= shadowUV.z;
	
	float shadow = 1.0f;
#ifndef NEVER_USE_VSM
	if(shadowParam.flag & SHADOW_PARAM_FLAG_USE_VSM)
		shadow = saturate( VarianceShadow(SpotLightMomentShadowMapAtlas, shadowUV.xy, depth) );
	else
#endif
#ifdef USE_SHADOW_INVERTED_DEPTH
		shadow = saturate( Shadowing(SpotLightShadowMapAtlas, shadowUV.xy, depth + bias) );
#else
		shadow = saturate( Shadowing(SpotLightShadowMapAtlas, shadowUV.xy, depth - bias) );
#endif

	float3 shadowColor = shadowParam.color.rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

	float shadowStrength = shadowParam.color.a;

	float3 ret = lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
	return float4(ret, shadow);
}

float4 RenderDirectionalLightShadow(uint lightIndex, float3 vertexWorldPos)
{
	uint shadowIndex = GetShadowIndex(DirectionalLightOptionalParamIndex[lightIndex]);

	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), DirectionalLightShadowViewProjMatrix[shadowIndex].mat );
	shadowUV /= shadowUV.w;

	uint lightCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);

	shadowUV.x = ((shadowUV.x / 2.0f) + 0.5f + float(shadowIndex)) * rcp((float)lightCount);
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	ParsedShadowParam shadowParam;
	ParseShadowParam(shadowParam, DirectionalLightShadowParams[shadowIndex]);

	float bias	= shadowParam.bias;
	float depth	= shadowUV.z;
	
	float shadow = 1.0f;
#ifndef NEVER_USE_VSM
	if(shadowParam.flag & SHADOW_PARAM_FLAG_USE_VSM)
		shadow = saturate( VarianceShadow(DirectionalLightMomentShadowMapAtlas, shadowUV.xy, depth) );
	else
#endif
#ifdef USE_SHADOW_INVERTED_DEPTH
		shadow = saturate( Shadowing(DirectionalLightShadowMapAtlas, shadowUV.xy, depth + bias) );
#else
		shadow = saturate( Shadowing(DirectionalLightShadowMapAtlas, shadowUV.xy, depth - bias) );
#endif

	float3 shadowColor = shadowParam.color.rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

	float shadowStrength = shadowParam.color.a;

	float3 ret = lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
	return float4(ret, shadow);
}

uint ComputeFaceIndex(float3 dir)
{
	// PointLight::ComputeViewProjMatrix 참고

	uint res = 0;

	if( (abs(dir.x) > abs(dir.y)) && (abs(dir.x) > abs(dir.z)) )
		res = dir.x >= 0.0f ? 2 : 3;
	else if( (abs(dir.y) > abs(dir.x)) && (abs(dir.y) > abs(dir.z)) )
		res = dir.y >= 0.0f ? 4 : 5;
	else
		res = dir.z >= 0.0f ? 0 : 1;

	return res;
}

float4 RenderPointLightShadow(uint lightIndex, float3 vertexWorldPos, float3 lightDir, float shadowDistanceTerm)
{
	uint faceIndex		= ComputeFaceIndex(-lightDir);
	uint shadowIndex	= GetShadowIndex(PointLightOptionalParamIndex[lightIndex]);

	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), PointLightShadowViewProjMatrix[shadowIndex].mat[faceIndex] );
	shadowUV /= shadowUV.w;

	shadowUV.x = (shadowUV.x / 2.0f) + 0.5f;
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	float oneShadowMapSize	= float(1 << GetNumOfPointLight(shadowGlobalParam_packedPowerOfTwoShadowResolution));
	float underScanSize		= asfloat(PointLightShadowParams[shadowIndex].z);

	ParsedShadowParam shadowParam;
	ParseShadowParam(shadowParam, PointLightShadowParams[shadowIndex].xy);

	shadowUV.xy *= ((oneShadowMapSize - (2.0f * underScanSize)) / oneShadowMapSize).xx;
	shadowUV.xy += (underScanSize / oneShadowMapSize).xx;

	shadowUV.y += (float)faceIndex;
	shadowUV.y *= rcp(6); //1.0f / 6.0f;

	uint lightCount = GetNumOfPointLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);

	shadowUV.x += (float)shadowIndex;
	shadowUV.x *= rcp((float)lightCount);//(1.0f / (float)lightCount);

	float bias	= lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * shadowParam.bias;
	float depth	= shadowUV.z;
	
	float shadow = 1.0f;
#ifdef USE_SHADOW_INVERTED_DEPTH
		shadow = saturate( Shadowing(PointLightShadowMapAtlas, shadowUV.xy, depth + bias) );
#else
		shadow = saturate( Shadowing(PointLightShadowMapAtlas, shadowUV.xy, depth - bias) );
#endif

	float3 shadowColor = shadowParam.color.rgb;
	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowColor, float3(1.0f, 1.0f, 1.0f), shadow);

#ifndef NOT_USE_SHADOW_STRENGTH
	float shadowStrength = shadowParam.color.a;
#else
	float shadowStrength = 1.0f;
#endif
	float3 ret = lerp(float3(1.0f, 1.0f, 1.0f), result, shadowStrength);
	return float4(ret, shadow);
}


#endif
