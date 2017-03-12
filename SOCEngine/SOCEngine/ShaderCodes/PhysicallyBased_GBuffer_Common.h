//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__
#define __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__

#include "ShaderCommon.h"
#include "PhysicallyBased_Common.h"
#include "EnvIBL.h"
#include "TBDRInput.h"

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
	float3 albedo = float3(0.0f, 0.0f, 0.0f);
	{
		float3 mtlMainColor	= GetMaterialMainColor().rgb;
		float3 diffuseTex	= diffuseMap.Sample(GBufferDefaultSampler, uv).rgb;

		albedo = lerp(mtlMainColor, diffuseTex * mtlMainColor, HasDiffuseMap());
	}
	float occlusion = occlusionMap.Sample(GBufferDefaultSampler, uv).x;

	float3 normal = normalize(worldNormal) * 0.5f + 0.5f;
	float roughness = 0.0f;
	{
		float roughnessTex = roughnessMap.Sample(GBufferDefaultSampler, uv).x;
		float matRoughness = GetMaterialRoughness();

		roughness = lerp(matRoughness, roughnessTex, HasRoughnessMap());
	}

	float specularity = GetMaterialSpecularity();
	float3 emissiveColor = float3(0.0f, 0.0f, 0.0f);
	{
		float3 mtlEmissiveColor	= GetMaterialEmissiveColor();
		float3 emissiveTex		= emissionMap.Sample(GBufferDefaultSampler, uv).rgb;

		emissiveColor = lerp(mtlEmissiveColor, emissiveTex * mtlEmissiveColor, HasEmissionMap());
	}

	float height = heightMap.Sample(GBufferDefaultSampler, uv).x;
	{
	}
		
	float metallic = 0.0f;
	{
		float metallicTex = metallicMap.Sample(GBufferDefaultSampler, uv).x;
		float mtlMetallic = GetMaterialMetallic();

		metallic = lerp(mtlMetallic, metallicTex, HasMetallicMap());
	}

	albedo_occlusion.rgb				= ToLinear(albedo, GetGamma());
	albedo_occlusion.a					= occlusion;
	velocity_metallic_specularity.rg	= velocity * 0.5f + 0.5f;
	velocity_metallic_specularity.b		= metallic;
	velocity_metallic_specularity.a		= specularity;
	normal_roughness.rgb				= normal;
	normal_roughness.a					= roughness;
	emission_materialFlag.rgb			= ToLinear(emissiveColor, GetGamma());
	emission_materialFlag.a				= float(GetMaterialFlag()) * rcp(255.0f);
}

#endif
