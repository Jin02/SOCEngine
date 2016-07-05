//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__
#define __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__

#include "ShaderCommon.h"
#include "PhysicallyBased_Common.h"
#include "EnvIBL.h"

struct GBuffer
{
	float4 albedo_occlusion			: SV_Target0;
	float4 motionXY_height_metallic		: SV_Target1;
	float4 normal_roughness 		: SV_Target2;
	float4 emission_specularity		: SV_Target3;
};

SamplerState GBufferDefaultSampler 		: register( s0 );


void MakeGBuffer(float3 worldNormal, float2 uv,
		 out float4 albedo_occlusion, out float4 motionXY_height_metallic, out float4 normal_roughness, out float4 emission_specularity)
{
	float3 albedo = float3(0.0f, 0.0f, 0.0f);
	{
		float3 mtlMainColor	= GetMaterialMainColor().rgb;
		float3 diffuseTex	= DiffuseMap.Sample(GBufferDefaultSampler, uv).rgb;

		albedo = lerp(mtlMainColor, diffuseTex * mtlMainColor, HasDiffuseMap());
	}
	float occlusion = OcclusionMap.Sample(GBufferDefaultSampler, uv).x;

	float3 normal = normalize(worldNormal) * 0.5f + 0.5f;
	float roughness = 0.0f;
	{
		float roughnessTex = RoughnessMap.Sample(GBufferDefaultSampler, uv).x;
		float matRoughness = GetMaterialRoughness();

		roughness = lerp(matRoughness, roughnessTex, HasRoughnessMap());
	}

	float specularity	= GetMaterialSpecularity();
	float3 emissiveColor	= float3(0.0f, 0.0f, 0.0f);
	{
		float3 mtlEmissiveColor	= GetMaterialEmissiveColor();
		float3 emissiveTex	= EmissionMap.Sample(GBufferDefaultSampler, uv).rgb;

		emissiveColor = lerp(mtlEmissiveColor, emissiveTex * mtlEmissiveColor, HasEmissionMap());
	}

	float height = HeightMap.Sample(GBufferDefaultSampler, uv).x;

	float2 motion = float2(0.0f, 0.0f);
	{

	}
	
	float metallic = 0.0f;
	{
		float metallicTex = MetallicMap.Sample(GBufferDefaultSampler, uv).x;
		float mtlMetallic = GetMaterialMetallic();

		metallic = lerp(mtlMetallic, metallicTex, HasMetallicMap());
	}

	albedo_occlusion.rgb		= albedo;
	albedo_occlusion.a		= occlusion;
	motionXY_height_metallic.rg	= motion;
	motionXY_height_metallic.b	= height;
	motionXY_height_metallic.a	= metallic;
	normal_roughness.rgb		= normal;
	normal_roughness.a		= roughness;
	emission_specularity.rgb	= emissiveColor;
	emission_specularity.a		= specularity;
}

#endif
