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

uint Float4ToUint(float4 value)
{
	value *= 255.0f;

	uint4 ret;
	ret.x = (uint(value.x) & 0x000000FF);
	ret.y = (uint(value.y) & 0x000000FF) << 8;
	ret.z = (uint(value.z) & 0x000000FF) << 16;
	ret.w = (uint(value.w) & 0x000000FF) << 24;

	return ret.w | ret.z | ret.y | ret.x;
}

float4 UintToFloat4(uint value)
{
	float4 ret;
	ret.x = float( value & 0x000000FF);
	ret.y = float((value & 0x0000FF00) >> 8);
	ret.z = float((value & 0x00FF0000) >> 16);
	ret.w = float((value & 0xFF000000) >> 24);

	return (ret / 255.0f);
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
	outParam.color		= UintToFloat4(shadowParam.y);
}

#endif