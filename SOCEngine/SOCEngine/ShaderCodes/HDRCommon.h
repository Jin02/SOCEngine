#ifndef __SOC_HDR_COMMON_H__
#define __SOC_HDR_COMMON_H__

#include "ShaderCommon.h"

cbuffer HDRGlobalParam : register(b1)
{
	uint	hdr_packedDeltaTime;
	float	hdr_exposureStrength;
	float	hdr_exposureSpeed;

	float	bloom_threshold;
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

float GetDeltaTime()
{
	return f16tof32(hdr_packedDeltaTime & 0xffff);
}

float GetInvDeltaTime()
{
	return f16tof32(hdr_packedDeltaTime >> 16);
}

#endif
