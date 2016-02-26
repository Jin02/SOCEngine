#ifndef __SOC_ENV_CUBE_MAP_FILTER_H__
#define __SOC_ENV_CUBE_MAP_FILTER_H__

#include "../GlobalDefine.h"
#include "ShaderCommon.h"

SamplerState skyCubeMapSampler	: register( s4  );
TextureCube	skyCubeMap			: register( t12 );

cbuffer SkyMapInfoParam			: register( b7 )
{
	float	skyMapInfoParam_maxMipCount;
	uint	skyMapInfoParam_isSkyLightOn;
	uint	skyMapInfoParam_isDynamicSkyLight;
	float	skyMapInfoParam_blendFraction;
};

float ComputeReflectionCaptureRoughnessFromMip( float Mip )
{
	const float HardcodedNumCaptureArrayMips = 7;
	float LevelFrom1x1 = HardcodedNumCaptureArrayMips - 1 - Mip;

	return exp2( ( 1.0f - LevelFrom1x1 ) / 1.2f );
}

half ComputeReflectionCaptureMipFromRoughness(half Roughness)
{
	float LevelFrom1x1 = 1.0f - 1.2f * log2(Roughness);

	const half HardcodedNumCaptureArrayMips = 7;
	return HardcodedNumCaptureArrayMips - 1 - LevelFrom1x1;
}

float3 GetSkyLightReflection(float3 reflectDir, float roughness, uniform bool isDynamicSkyLight)
{
	float absSpecularMip	= ComputeReflectionCaptureMipFromRoughness(roughness);
	float3 reflection		= skyCubeMap.SampleLevel(skyCubeMapSampler, reflectDir, absSpecularMip).rgb;

	if(isDynamicSkyLight)
	{
		const float MaxMip = skyMapInfoParam_maxMipCount;
		float3 LowFrequencyReflection = skyCubeMap.SampleLevel(skyCubeMapSampler, reflectDir, MaxMip).rgb;
		float LowFrequencyBrightness = Luminance(LowFrequencyReflection);
		reflection /= max(LowFrequencyBrightness, 0.00001f);
	}

	return reflection;
}



#endif