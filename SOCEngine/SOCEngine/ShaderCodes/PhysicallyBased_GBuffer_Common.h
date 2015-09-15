//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__
#define __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__

#include "PhysicallyBased_Common.h"

struct GBuffer
{
	float4 albedo_emission			: SV_Target0;
	float4 specular_metallic		: SV_Target1;
	float4 normal_roughness 		: SV_Target2;
};

SamplerState GBufferDefaultSampler 	: register( s0 );

#endif