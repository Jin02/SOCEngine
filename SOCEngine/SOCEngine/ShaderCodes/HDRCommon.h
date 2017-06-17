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


#endif
