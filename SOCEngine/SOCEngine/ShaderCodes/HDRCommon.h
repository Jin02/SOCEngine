#ifndef __SOC_HDR_COMMON_H__
#define __SOC_HDR_COMMON_H__

#include "ShaderCommon.h"

cbuffer HDRGlobalParam : register(b1)
{
	float hdr_timeDelta;
	float hdr_exposureStrength;
	float hdr_exposureSpeed;

	float bloom_threshold;
};

float GetExposureStrength()
{
	return hdr_exposureStrength;
}

float GetExposureSpeed()
{
	return hdr_exposureSpeed;
}

float GetBloomThreshold()
{
	return bloom_threshold;
}

float GetTimeDelta()
{
	return hdr_timeDelta;
}

#endif
