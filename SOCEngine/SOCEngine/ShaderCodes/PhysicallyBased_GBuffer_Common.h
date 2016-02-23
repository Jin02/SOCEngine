//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__
#define __SOC_PHYSICALLY_BASED_GBUFFER_COMMON_H__

#include "ShaderCommon.h"
#include "PhysicallyBased_Common.h"

struct GBuffer
{
	float4 albedo_sunOcclusion		: SV_Target0;
	float4 specular_metallic		: SV_Target1;
	float4 normal_roughness 		: SV_Target2;
	float4 emission					: SV_Target3;
};

SamplerState GBufferDefaultSampler 	: register( s0 );


#if defined(USE_PBR_TEXTURE)
void MakeGBuffer(float4 diffuseTex, float4 normalWithRoughness, float4 specularTex,
				out float4 albedo_sunOcclusion, out float4 specular_metallic, out float4 normal_roughness, out float4 emission)
#else
void MakeGBuffer(float4 diffuseTex, float3 normal, float4 specularTex,
				 out float4 albedo_sunOcclusion, out float4 specular_metallic, out float4 normal_roughness, out float4 emission)
#endif
{
#if defined(USE_PBR_TEXTURE)
	float3 normal = normalWithRoughness.rgb;
#endif

	bool hasDiffuseMap		= HasDiffuseTexture();
	bool hasSpecularMap		= HasSpecularTexture();
	
	float metallic, roughness;
	Parse_Metallic_Roughness(metallic, roughness);

	float3 emissionColor	= material_emissionColor.rgb;
	float3 mainColor		= abs(material_mainColor);
	float3 albedo			= diffuseTex.rgb * mainColor + emissionColor;
	albedo_sunOcclusion.rgb	= lerp(mainColor + emissionColor, albedo, hasDiffuseMap);

	float3 specular			= specularTex.rgb;
	specular_metallic.rgb	= lerp(float3(0.05f, 0.05f, 0.05f), specular, hasSpecularMap);

	float3 compressedNormal = normalize(normal) * 0.5f + 0.5f;
	normal_roughness.rgb	= compressedNormal;

	emission.rgb			= material_emissionColor.rgb;

#if defined(USE_PBR_TEXTURE)
	albedo_sunOcclusion.a	= 0.0f;
	specular_metallic.a 	= specularTex.a;
	normal_roughness.a		= normalWithRoughness.a;
	emission.a				= 0.0f;
#else
	albedo_sunOcclusion.a	= 0.0f;
	specular_metallic.a 	= metallic;
	normal_roughness.a		= roughness;
	emission.a				= 0.0f;
#endif
}

#endif