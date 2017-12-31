//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "TBDRInput.h"
#include "CommonConstBuffer.h"

cbuffer SunShaftParam : register( b0 )
{
	uint	bufferSize;
	uint	sunShaft_UV;
	uint	packedDLIndexWithAspect;
	uint	packedSizeWithIntensity;
};
uint2 GetSize()
{
	return uint2(bufferSize & 0xffff, bufferSize >> 16);
}
float2 GetSunUV()
{
	return float2(f16tof32(sunShaft_UV & 0xffff), f16tof32(sunShaft_UV >> 16));
}
uint GetDirectionalLightIndex()
{
	return packedDLIndexWithAspect & 0xffff;
}
float GetAspect()
{
	return f16tof32(packedDLIndexWithAspect >> 16);
}
float GetCircleSize()
{
	return f16tof32(packedSizeWithIntensity & 0xffff);
}
float GetCircleIntensity()
{
	return f16tof32(packedSizeWithIntensity >> 16);
}

float DrawCircle(float2 uv, float2 circlePosUV, float size, float aspect, float intensity)
{
	float circle = saturate(length((uv - circlePosUV) * float2(aspect, 1.0f)) / size);
	return saturate((1.0f - circle * circle) * intensity);
}

float4 OcclusionMapPS(PS_INPUT input) : SV_Target
{
	uint lightIndex = GetDirectionalLightIndex();
	float3 color	= DirectionalLightColorBuffer[lightIndex].rgb;
	float3 lightDir = GetDirectionalLightDir(lightIndex);
	float3 viewDir	= float3(camera_viewMat._13, camera_viewMat._23, camera_viewMat._33);
	float3 circle	= DrawCircle(input.uv, GetSunUV(), GetCircleSize(), GetAspect(), GetCircleIntensity()) * color * saturate(dot(-lightDir, viewDir));

	return float4(circle, 1.0f);
}

#if (MSAA_SAMPLES_COUNT > 1)
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		OcclusionMap : register( t27 );
#else
Texture2D<float4>							OcclusionMap : register( t27 );
#endif

Texture2D<float4>							InputColorMap : register( t28 );

SamplerState DefaultSampler	: register( s0 );


#define NUM_SAMPLES 32
static const float Weight		= 1.0f / float(NUM_SAMPLES);
static const float ShaftDecay	= 1.0f;// - 0.0f / float(NUM_SAMPLES);

float4 SunShaftPS(PS_INPUT input) : SV_Target
{
	float3 resultColor	= 0.0f;
	float decay			= 1.0f;
	uint2 size			= GetSize();

	for (uint i = 0; i < NUM_SAMPLES; ++i)
	{
		float2 nextUV = lerp(input.uv, GetSunUV(), float(i) / float(NUM_SAMPLES-1));

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
		float3 sampledColor =  OcclusionMap.Load(input.uv * size, 0).rgb;
#else
		float3 sampledColor =  OcclusionMap.Load(uint3(input.uv * size, 0)).rgb;
#endif

		sampledColor *= decay * Weight;

		resultColor += sampledColor;
		decay *= ShaftDecay;
	}
	
	float3 inputColor = InputColorMap.Sample(DefaultSampler, input.uv).rgb;
	return float4(saturate(inputColor + resultColor), 1.0f);
}