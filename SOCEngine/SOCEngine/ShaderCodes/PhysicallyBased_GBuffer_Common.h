//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__
#define __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__

#include "ShaderCommon.h"
#include "PhysicallyBased_Common.h"

struct GBuffer
{
	float4 albedo_emission			: SV_Target0;
	float4 specular_metallic		: SV_Target1;
	float4 normal_roughness 		: SV_Target2;
};

SamplerState GBufferDefaultSampler 	: register( s0 );


#if defined(USE_PBR_TEXTURE)
void MakeGBuffer(float4 diffuseTex, float4 normalWithRoughness, float4 specularTex,
				 out float4 albedo_emission, out float4 specular_metallic, float4 normal_roughness)
#else
void MakeGBuffer(float4 diffuseTex, float3 normal, float4 specularTex,
				 out float4 albedo_emission, out float4 specular_metallic, out float4 normal_roughness)
#endif
{
#if defined(USE_PBR_TEXTURE)
	float3 normal = normalWithRoughness.rgb;
#endif

	bool hasDiffuseMap		= HasDiffuseTexture();
	bool hasSpecularMap		= HasSpecularTexture();
	
	float metallic, roughness, emission;
	Parse_Metallic_Roughness_Emission(metallic, roughness, emission);

	float3 mainColor		= abs(material_mainColor);
	float3 albedo			= diffuseTex.rgb * mainColor;
	albedo_emission.rgb		= lerp(float3(1.f, 1.f, 1.f) * mainColor, albedo, hasDiffuseMap);

	float3 specular			= specularTex.rgb;
	specular_metallic.rgb	= lerp(float3(0.05f, 0.05f, 0.05f), specular, hasSpecularMap);

	float3 compressedNormal = normalize(normal) * 0.5f + 0.5f;
	normal_roughness.rgb	= compressedNormal;

#if defined(USE_PBR_TEXTURE)
	albedo_emission.a		= diffuseTex.a;
	specular_metallic.a 	= specularTex.a;
	normal_roughness.a		= normalWithRoughness.a;
#else
	albedo_emission.a		= emission;
	specular_metallic.a 	= metallic;
	normal_roughness.a		= roughness;
#endif
}

#endif