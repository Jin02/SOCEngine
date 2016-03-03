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

cbuffer Camera : register( b2 )
{
	matrix	camera_viewMat;				// or InvNearFarViewProj
	matrix	camera_viewProjMat;
	float4	camera_worldPos;
};

cbuffer Material : register( b3 )		//PhysicallyBasedMaterial
{
	uint	material_mainColor_alpha;
	uint	material_emissiveColor_Metallic;
	uint	material_roughness_specularity_existTextureFlag;
	uint	material_flag;

	float2 	material_uvTiling0;
	float2 	material_uvOffset0;
	float2 	material_uvTiling1;
	float2 	material_uvOffset1;
};

Texture2D diffuseMap			: register( t8 );
Texture2D normalMap				: register( t9 );
Texture2D opacityMap			: register( t10 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering
Texture2D heightMap				: register( t11 );
Texture2D metallicMap			: register( t12 );
Texture2D occlusionMap			: register( t13 );
Texture2D roughnessMap			: register( t14 );
Texture2D emissionMap			: register( t15 );

float4 GetMaterialMainColor()
{
	return float4(	(material_mainColor_alpha & 0xff000000) >> 24,
					(material_mainColor_alpha & 0x00ff0000) >> 16,
					(material_mainColor_alpha & 0x0000ff00) >> 8,
					(material_mainColor_alpha & 0x000000ff) >> 0	) / 255.0f;
}

float3 GetMaterialEmissiveColor()
{
	return float3(	(material_emissiveColor_Metallic & 0xff000000) >> 24,
					(material_emissiveColor_Metallic & 0x00ff0000) >> 16,
					(material_emissiveColor_Metallic & 0x0000ff00) >> 8	) / 255.0f;
}

float GetMaterialMetallic()
{
	return float(material_emissiveColor_Metallic & 0x000000ff) / 255.0f;
}

float GetMaterialSpecularity()
{
	return float( (material_roughness_specularity_existTextureFlag & 0x00ff0000) >> 16 ) / 255.0f;
}

float GetMaterialRoughness()
{
	return float( (material_roughness_specularity_existTextureFlag & 0xff000000) >> 24 ) / 255.0f;
}

uint GetMaterialExistTextureFlag()
{
	return material_roughness_specularity_existTextureFlag & 0x0000ffff;
}

uint GetMaterialFlag()
{
	return material_flag;
}


bool HasDiffuseMap()	{	return (GetMaterialExistTextureFlag() & (1 << 0));	}
bool HasNormalMap()		{	return (GetMaterialExistTextureFlag() & (1 << 1));	}
bool HasOpacityMap()	{	return (GetMaterialExistTextureFlag() & (1 << 2));	}
bool HasHeightMap()		{	return (GetMaterialExistTextureFlag() & (1 << 3));	}
bool HasMetallicMap()	{	return (GetMaterialExistTextureFlag() & (1 << 4));	}
bool HasOcclusionMap()	{	return (GetMaterialExistTextureFlag() & (1 << 5));	}
bool HasRoughnessMap()	{	return (GetMaterialExistTextureFlag() & (1 << 6));	}
bool HasEmissionMap()	{	return (GetMaterialExistTextureFlag() & (1 << 7));	}

#endif