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
	uint	material_metallic_roughness_emission;

	float2 	material_uvTiling;
	float2 	material_uvOffset;
};

Texture2D diffuseTexture			: register( t8 );
Texture2D normalTexture				: register( t9 );
Texture2D specularTexture			: register( t10 );
Texture2D opacityTexture			: register( t11 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering

float3 ComputeFaceNormal(float3 position)
{
    return cross(ddx_coarse(position), ddy_coarse(position));
}

float3 DecodeNormalTexture(in Texture2D tex, in float2 uv, in SamplerState samplerState)
{
	float3 norm = tex.Sample(samplerState, uv).xyz;
	norm *= 2.0f; norm -= float3(1.0f, 1.0f, 1.0f);

	return norm;
}

void Parse_Metallic_Roughness_Emission(in uint material_mre,
									   out float metallic,
									   out float roughness,
									   out float emission)
{
	uint scaledMetallic		= material_mre & 0x3ff00000;
	uint scaledRoughness	= material_mre & 0x000ffc00;
	uint scaledEmission		= material_mre & 0x000003ff;

	metallic	= (float)scaledMetallic	/ 1024.0f;
	roughness	= (float)scaledRoughness	/ 1024.0f;
	emission	= (float)scaledEmission	/ 1024.0f;
}

#endif