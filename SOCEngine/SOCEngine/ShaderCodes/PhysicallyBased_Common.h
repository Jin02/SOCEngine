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
	float	material_metallic;
	float 	material_roughness;
	float 	material_fresnel0;
	float2 	material_uvTiling;
};

Texture2D diffuseTexture			: register( t8 );
Texture2D normalTexture				: register( t9 );
Texture2D specularTexture			: register( t10 );
Texture2D opacityTexture			: register( t11 ); //inverse, 0 is opcity 100%, 1 is 0%. used in Transparency Rendering

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

#endif