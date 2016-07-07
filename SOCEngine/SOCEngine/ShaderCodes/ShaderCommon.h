//NOT_CREATE_META_DATA

#ifndef __SOC_SHADER_COMMON_H__
#define __SOC_SHADER_COMMON_H__

#include "../GlobalDefine.h"

#define DEG_2_RAD(X) PI * X / 180.0f

uint GetNumOfPointLight(uint packedNumOfLights)
{
	return packedNumOfLights >> 21;
}

uint GetNumOfSpotLight(uint packedNumOfLights)
{
	return (packedNumOfLights >> 10) & 0x7FF;
}

uint GetNumOfDirectionalLight(uint packedNumOfLights)
{
	return packedNumOfLights & 0x000003FF;
}

uint ToUint(float4 value)
{
	return	(uint(value.w) & 0x000000FF) << 24	|
			(uint(value.z) & 0x000000FF) << 16	|
			(uint(value.y) & 0x000000FF) << 8	|
			(uint(value.x) & 0x000000FF);
}

uint Float4ColorToUint(float4 value)
{
	return ToUint(value * 255.0f);
}

float4 ToFloat4(uint value)
{
	return float4(	float((value & 0x000000FF) >> 0),
					float((value & 0x0000FF00) >> 8),
					float((value & 0x00FF0000) >> 16),
					float((value & 0xFF000000) >> 24)	);
}

float4 RGBA8UintColorToFloat4(uint value)
{
	return ToFloat4(value) / 255.0f;
}

float smoothStep(float low, float high, float v)
{
	return clamp( (v - low) / (high - low), 0.0f, 1.0f);
}

struct ParsedShadowParam
{
	uint	lightIndex;
	uint	flag;
	float	bias;
	float4	color;
};

void ParseShadowParam(out ParsedShadowParam outParam, uint2 shadowParam)
{
	outParam.lightIndex	= shadowParam.x >> 16;
	outParam.bias		= float(uint(shadowParam.x & 0xfff0) >> 4) / (8192.0f);
	outParam.flag		= shadowParam.x & 0xf;
	outParam.color		= RGBA8UintColorToFloat4(shadowParam.y);
}

float Luminance(float3 linearColor)
{
	return dot(linearColor, float3(0.3f, 0.59f, 0.11f));
}

float SimpleNoise(in float3 co)
{
    return frac(sin(dot(co, float3(12.9898f, 78.233f, 31.323f))) * 43758.5453f);
}

#endif