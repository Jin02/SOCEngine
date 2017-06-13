//EMPTY_META_DATA

#ifndef __SOC_SHADOW_H__
#define __SOC_SHADOW_H__

#include "TBDRInput.h"
#include "ShaderCommon.h"

#define SHADOW_KERNEL_LEVEL				4
#define SHADOW_KERNEL_WIDTH				2 * SHADOW_KERNEL_LEVEL + 1
#define SHADOW_PARAM_FLAG_USE_VSM		1

#define BLOCKER_SEARCH_STEP_COUNT		3
#define BLOCKER_SEARCH_DIM				(BLOCKER_SEARCH_STEP_COUNT * 2 + 1)
#define BLOCKER_SEARCH_COUNT			(BLOCKER_SEARCH_DIM * BLOCKER_SEARCH_DIM)

SamplerComparisonState shadowSamplerCmpState	:	register( s2 );
SamplerState shadowSamplerState					:	register( s3 );
SamplerState pointSamplerState					:	register( s4 );


struct ShadowParam
{
	float	lightNear;

	float	underScanSize;
	float	softness;

	uint	lightIndex;
	float	bias;
	uint	flag;

	float3	color;
	float	strength;
};

void ParseShadowParam(out ShadowParam outParam, uint4 param)
{
	outParam.lightNear		= asfloat(param.x);

	outParam.underScanSize	= f16tof32(param.y >> 16);
	outParam.softness		= f16tof32(param.y & 0xffff);

	outParam.lightIndex		= (param.z >> 16);
	outParam.bias			= float((param.z & 0x0000ff00) >> 8) / 1020.0f;
	outParam.flag			= (param.z & 0x000000ff);

	float4 shadowColor		= RGBA8UintColorToFloat4(param.w);
	outParam.color			= shadowColor.rgb;
	outParam.strength		= shadowColor.a;
}

float2 ComputeStepUV(uint lightCapacityCount, float softness, uniform uint faceCount)
{
	float oneShadowMapSize	= float(1 << GetNumOfSpotLight(shadowGlobalParam_packedPowerOfTwoShadowResolution));
	float2 atlasMapSize	= float2(lightCapacityCount, float(faceCount)) * oneShadowMapSize;
	float2 stepUV		= softness * rcp(atlasMapSize);
	
	return stepUV;
}


float PCF(Texture2D<float> atlas, float2 uv, float depth, float2 stepUV)
{
	float shadow = 0.0f;
	[unroll] for(int i=-SHADOW_KERNEL_LEVEL; i<=SHADOW_KERNEL_LEVEL; ++i)
	{
		[unroll] for(int j=-SHADOW_KERNEL_LEVEL; j<=SHADOW_KERNEL_LEVEL; ++j)
		{
			float2 offset = float2(i, j) * stepUV;
			shadow += (atlas.SampleCmpLevelZero(shadowSamplerCmpState, uv + offset, depth).x);
		}
	}

	shadow /= (float)(SHADOW_KERNEL_WIDTH * SHADOW_KERNEL_WIDTH);
	return shadow;
}

void FindBlocker(out float outAccumBlockerDepth, out uint outNumBlockers,
				 Texture2D<float> atlas, float2 uv, float lightVPDepth, float2 stepUV)
{
	for(uint x = -BLOCKER_SEARCH_STEP_COUNT; x <= BLOCKER_SEARCH_STEP_COUNT; ++x)
	{
		for(uint y = -BLOCKER_SEARCH_STEP_COUNT; y <= BLOCKER_SEARCH_STEP_COUNT; ++y)
		{
			float2 offset = float2(x, y) * stepUV;
			float shadowMapDepth = atlas.SampleLevel(pointSamplerState, uv + offset, 0);

			if (shadowMapDepth > lightVPDepth)
			{
				outAccumBlockerDepth += shadowMapDepth;
				outNumBlockers++;
			}
		}
	}
}

struct PCSSParam
{
	float lightViewDepth;
	float lightNear;
	float softness;
	float2 atlasMapSize;
	float invProjMat_34;
	float invProjMat_44;
};

float InvertLightProjDepthToView(float depth, float invProjMat_34, float invProjMat_44)
{
	return 1.0f / (depth * invProjMat_34 + invProjMat_44);
}

float2 ComputeSearchAreaRadiusUV(float softness, float lightViewDepth, float lightNear)
{
	return softness.xx * (lightViewDepth - lightNear) / lightViewDepth;
}

float PCSS(Texture2D<float> atlas, float2 uv, float lightVPDepth, PCSSParam param)
{
	float accumBlockerDepth = 0.0f;
	uint blockerCount	= 0;
	
	float2 searchAreaUV	= ComputeSearchAreaRadiusUV(param.softness, param.lightViewDepth, param.lightNear) * rcp(param.atlasMapSize);
	FindBlocker(accumBlockerDepth, blockerCount, atlas, uv, lightVPDepth, searchAreaUV);

	if(blockerCount == 0)							return 1.0f;
	else if(blockerCount == BLOCKER_SEARCH_COUNT)	return 0.0f;

	float avgDepth		= InvertLightProjDepthToView(accumBlockerDepth / float(blockerCount),
							     param.invProjMat_34, param.invProjMat_44);
	float2 penumbraRadiusUV	= (param.softness.xx * (param.lightViewDepth - avgDepth) / avgDepth);
	float2 filterRadiusUV	= (penumbraRadiusUV * param.lightNear / param.lightViewDepth);

	float2 stepUV = filterRadiusUV * rcp(param.atlasMapSize);
	return PCF(atlas, uv, lightVPDepth, stepUV);
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
	float2 moment = RecombinePrecision(momentShadowMapAtlas.SampleLevel(shadowSamplerState, uv, 0));
	return ChebyshevUpperBound(moment, depth);
}

float4 RenderSpotLightShadow(uint lightIndex, float3 vertexWorldPos, float shadowDistanceTerm)
{
	uint shadowIndex = GetShadowIndex(SpotLightOptionalParamIndex[lightIndex]);

	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), SpotLightShadowViewProjMatrix[shadowIndex].mat);
	shadowUV /= shadowUV.w;

	uint lightCapacityCount = GetNumOfSpotLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	shadowUV.x = ((shadowUV.x / 2.0f) + 0.5f + float(shadowIndex)) * rcp((float)lightCapacityCount);
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	ShadowParam shadowParam;
	ParseShadowParam(shadowParam, SpotLightShadowParams[shadowIndex]);

	float bias		= lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * shadowParam.bias;
	float depth		= shadowUV.z;
	float2 stepUV	= ComputeStepUV(lightCapacityCount, shadowParam.softness, 1);	
	float shadow	= saturate( PCF(SpotLightShadowMapAtlas, shadowUV.xy, depth + bias, stepUV) );

	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowParam.color, float3(1.0f, 1.0f, 1.0f), shadow);

	float3 ret = lerp(float3(1.0f, 1.0f, 1.0f), result, shadowParam.strength);
	return float4(ret, shadow);
}

float4 RenderDirectionalLightShadow(uint lightIndex, float3 vertexWorldPos)
{
	uint shadowIndex = GetShadowIndex(DirectionalLightOptionalParamIndex[lightIndex]);

	float4 shadowUV = mul( float4(vertexWorldPos, 1.0f), DirectionalLightShadowViewProjMatrix[shadowIndex].mat );
	shadowUV /= shadowUV.w;

	uint lightCapacityCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);

	shadowUV.x = ((shadowUV.x / 2.0f) + 0.5f + float(shadowIndex)) * rcp((float)lightCapacityCount);
	shadowUV.y = (shadowUV.y /-2.0f) + 0.5f;

	ShadowParam shadowParam;
	ParseShadowParam(shadowParam, DirectionalLightShadowParams[shadowIndex]);

	float bias		= shadowParam.bias;
	float depth		= shadowUV.z;
	float2 stepUV	= ComputeStepUV(lightCapacityCount, shadowParam.softness, 1);	
	float shadow	= saturate( PCF(DirectionalLightShadowMapAtlas, shadowUV.xy, depth + bias, stepUV) );

	float3 result = lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowParam.color, float3(1.0f, 1.0f, 1.0f), shadow);
	float3 ret = lerp(float3(1.0f, 1.0f, 1.0f), result, shadowParam.strength);
	return float4(ret, shadow);
}

uint ComputeFaceIndex(float3 dir)
{
	// PointLight::MakeVPMatParam 참고

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

	ShadowParam shadowParam;
	ParseShadowParam(shadowParam, PointLightShadowParams[shadowIndex]);

	shadowUV.xy *= ((oneShadowMapSize - (2.0f * shadowParam.underScanSize)) / oneShadowMapSize).xx;
	shadowUV.xy += (shadowParam.underScanSize / oneShadowMapSize).xx;

	shadowUV.y += (float)faceIndex;
	shadowUV.y *= rcp(6); //1.0f / 6.0f;

	uint lightCapacityCount = GetNumOfPointLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);

	shadowUV.x += (float)shadowIndex;
	shadowUV.x *= rcp((float)lightCapacityCount);//(1.0f / (float)lightCapacityCount);

	float bias	= lerp(10.0f, 1.0f, saturate(5 * shadowDistanceTerm)) * shadowParam.bias;
	float depth	= shadowUV.z;

	float2 stepUV	= ComputeStepUV(lightCapacityCount, shadowParam.softness, 6);
	float shadow	= saturate( PCF(PointLightShadowMapAtlas, shadowUV.xy, depth + bias, stepUV) );

	float3 result	= lerp((float3(1.0f, 1.0f, 1.0f) - shadow.xxx) * shadowParam.color, float3(1.0f, 1.0f, 1.0f), shadow);
	float3 ret		= lerp(float3(1.0f, 1.0f, 1.0f), result, shadowParam.strength);
	return float4(ret, shadow);
}


#endif
