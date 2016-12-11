#ifndef __SOC_HDR_COMMON_H__
#define __SOC_HDR_COMMON_H__

#include "ShaderCommon.h"

cbuffer HDRGlobalParam : register(b0)
{
	float hdr_timeDelta;
	float hdr_gamma;

	float hdr_exposureKey;
	uint hdr_exposureSpeedWithBloomThreshold;	//half, half
};

float GetExposureSpeed()
{
	return f16tof32(hdr_exposureSpeedWithBloomThreshold >> 16);
}

float GetBloomThreshold()
{
	return f16tof32(hdr_exposureSpeedWithBloomThreshold & 0xffff);
}

#endif
