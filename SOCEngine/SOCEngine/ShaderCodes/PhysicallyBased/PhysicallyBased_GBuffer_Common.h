//NOT_CREATE_META_DATA

#include "PhysicallyBased_Common.h"

struct GBuffer
{
	float4 albedo_metallic			: SV_Target0;
	float4 specular_fresnel0		: SV_Target1;
	float4 normal_roughness 		: SV_Target2;
};

SamplerState GBufferDefaultSampler 	: register( s0 );