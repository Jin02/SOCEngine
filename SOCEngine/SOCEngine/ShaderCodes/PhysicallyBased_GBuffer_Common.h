//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__
#define __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__

#include "ShaderCommon.h"
#include "PhysicallyBased_Common.h"
#include "EnvIBL.h"

struct GBuffer
{
	float4 albedo_occlusion					: SV_Target0;
	float4 velocity_metallic_specularity	: SV_Target1;
	float4 normal_roughness 				: SV_Target2;
	float4 emission_materialFlag			: SV_Target3;
};

SamplerState GBufferDefaultSampler 	: register( s0 );


void MakeGBuffer(float3 worldNormal, float2 uv, float2 velocity,
		 out float4 albedo_occlusion, out float4 velocity_metallic_specularity, out float4 normal_roughness, out float4 emission_materialFlag)
{
	float3 albedo			= GetDiffuse(GBufferDefaultSampler, uv).rgb;
	float occlusion			= GetOcclusion(GBufferDefaultSampler, uv);
	float3 normal			= PackNormal(worldNormal);
	float roughness			= GetRoughness(GBufferDefaultSampler, uv);
	float specularity		= GetMaterialSpecularity();
	float3 emissiveColor	= GetEmissiveColor(GBufferDefaultSampler, uv);
//	float height			= GetHeight(GBufferDefaultSampler, uv);
	float metallic			= GetMetallic(GBufferDefaultSampler, uv);

	albedo_occlusion.rgb				= albedo;
	albedo_occlusion.a					= occlusion;
	velocity_metallic_specularity.rg	= velocity * 0.5f + 0.5f;
	velocity_metallic_specularity.b		= metallic;
	velocity_metallic_specularity.a		= specularity;
	normal_roughness.rgb				= normal;
	normal_roughness.a					= roughness;
	emission_materialFlag.rgb			= emissiveColor;
	emission_materialFlag.a				= float(GetMaterialFlag()) * rcp(255.0f);
}

#endif
