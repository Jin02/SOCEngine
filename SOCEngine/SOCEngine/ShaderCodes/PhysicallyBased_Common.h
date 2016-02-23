//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_COMMON_H__
#define __SOC_PHYSICALLY_BASED_COMMON_H__

#include "NormalMapping.h"

#if (MSAA_SAMPLES_COUNT > 1)
#define ALPHA_TEST_BIAS		0.003f
#else
#define ALPHA_TEST_BIAS		0.5f
#endif

cbuffer Transform : register( b1 )		//Object World
{
	matrix transform_world;
	matrix transform_worldInvTranspose;
};

cbuffer CameraMat : register( b2 )
{
	matrix	cameraMat_view;				// or InvNearFarViewProj
	matrix	cameraMat_viewProj;
};

cbuffer Material : register( b3 )		//PhysicallyBasedMaterial
{
	float3	material_mainColor;
	uint	material_alpha_metallic_roughness_flag;

	float4	material_emissionColor;

	float2 	material_uvTiling0;
	float2 	material_uvOffset0;
	float2 	material_uvTiling1;
	float2 	material_uvOffset1;
};

Texture2D diffuseTexture			: register( t8 );
Texture2D normalTexture				: register( t9 );
Texture2D specularTexture			: register( t10 );
Texture2D opacityTexture			: register( t11 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering

void Parse_Metallic_Roughness( out float metallic,
							   out float roughness)
{
	uint scaledMetallic		= (material_alpha_metallic_roughness_flag & 0x00ff0000) >> 16;
	uint scaledRoughness	= (material_alpha_metallic_roughness_flag & 0x0000ff00) >> 8;

	metallic	= (float)scaledMetallic		/ 255.0f;
	roughness	= (float)scaledRoughness	/ 255.0f;
}

float ParseMaterialAlpha()
{
	return ( (float)((material_alpha_metallic_roughness_flag & 0xff000000) >> 24) / 255.0f );
}

uint ParseMaterialFlag()
{
	return material_alpha_metallic_roughness_flag & 0x000000ff;
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

#endif