#ifndef __SOC_ENV_CUBE_MAP_FILTER_H__
#define __SOC_ENV_CUBE_MAP_FILTER_H__

#include "../GlobalDefine.h"
#include "MonteCarlo.h"
#include "ShaderCommon.h"

SamplerState skyCubeMapSampler	: register( s4  );
TextureCube	skyCubeMap			: register( t12 );

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

float3 GetSkyLightReflection(float3 reflectDir, float roughnessm, uniform bool isDynamicSkyLight)
{
	float absSpecularMip	= ComputeReflectionCaptureMipFromRoughness(roughness);
	float3 reflection		= cubeMap.SampleLevel(skyCubeMapSampler, reflectDir, absSpecularMip);

	if(isDynamicSkyLight)
	{
		float3 LowFrequencyReflection = skyCubeMap.SampleLevel(reflectDir, skyCubeMapSampler, maxMip).rgb;
		float LowFrequencyBrightness = Luminance(LowFrequencyReflection);
		reflection /= max(LowFrequencyBrightness, 0.00001f);
	}

	return reflection;
}



#endif