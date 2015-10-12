//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_COMMON_H__
#define __SOC_PHYSICALLY_BASED_COMMON_H__

#if (MSAA_SAMPLES_COUNT > 1)
#define ALPHA_TEST_BIAS		0.003f
#else
#define ALPHA_TEST_BIAS		0.5f
#endif

cbuffer Transform : register( b1 )		//Mesh
{
	matrix transform_world;
	matrix transform_worldView;
	matrix transform_worldViewProj;
};

cbuffer Material : register( b2 )		//PhysicallyBasedMaterial
{
	float3	material_mainColor;
	uint	material_alpha_metallic_roughness_emission;

	float2 	material_uvTiling0;
	float2 	material_uvOffset0;
	float2 	material_uvTiling1;
	float2 	material_uvOffset1;
};

Texture2D diffuseTexture			: register( t8 );
Texture2D normalTexture				: register( t9 );
Texture2D specularTexture			: register( t10 );
Texture2D opacityTexture			: register( t11 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering

float3 NormalMapping(float3 normalMapXYZ, float3 normal, float3 tangent, float2 uv)
{
	float3 binormal = normalize( cross(normal, tangent) );

	float3 texNormal = normalMapXYZ;
	texNormal *= 2.0f; texNormal -= float3(1.0f, 1.0f, 1.0f);

	float3x3 TBN = float3x3(normalize(binormal), normalize(tangent), normalize(normal));

	return normalize( mul(texNormal, TBN) );
}

void Parse_Metallic_Roughness_Emission(out float metallic,
									   out float roughness,
									   out float emission)
{
	uint scaledMetallic		= (material_alpha_metallic_roughness_emission & 0x00ff0000) >> 16;
	uint scaledRoughness	= (material_alpha_metallic_roughness_emission & 0x0000ff00) >> 8;
	uint scaledEmission		= (material_alpha_metallic_roughness_emission & 0x000000ff) >> 0;

	metallic	= (float)scaledMetallic		/ 255.0f;
	roughness	= (float)scaledRoughness	/ 255.0f;
	emission	= (float)scaledEmission		/ 255.0f;
}

float ParseMaterialAlpha()
{
	return ( (float)((material_alpha_metallic_roughness_emission & 0xff000000) >> 24) / 255.0f );
}

bool HasDiffuseTexture()
{
	return material_mainColor.r < 0.0f;
}

bool HasNormalTexture()
{
	return material_mainColor.g < 0.0f;
}

bool HasSpecularTexture()
{
	return material_mainColor.b < 0.0f;
}

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

	float3 albedo			= diffuseTex.rgb * abs(material_mainColor);
	albedo_emission.rgb		= lerp(float3(1.f, 1.f, 1.f), albedo, hasDiffuseMap);

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