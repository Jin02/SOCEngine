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
	matrix	camera_viewMat;			// or InvNearFarViewProj
	matrix	camera_viewProjMat;
	float4	camera_worldPos;
};

cbuffer Material : register( b3 )		//PhysicallyBasedMaterial
{
	uint	material_mainColor_alpha;
	uint	material_emissiveColor_Metallic;
	uint	material_roughness_specularity_existMapFlag;
	uint	material_flag;

	float2 	material_uvTiling0;
	float2 	material_uvOffset0;
	float2 	material_uvTiling1;
	float2 	material_uvOffset1;
};

Texture2D DiffuseMap			: register( t8 );
Texture2D NormalMap			: register( t9 );
Texture2D OpacityMap			: register( t10 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering
Texture2D HeightMap			: register( t11 );
Texture2D MetallicMap			: register( t12 );
Texture2D OcclusionMap			: register( t13 );
Texture2D RoughnessMap			: register( t33 );
Texture2D EmissionMap			: register( t34 );

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
	return float( material_emissiveColor_Metallic & 0x000000ff ) / 255.0f;
}

float GetMaterialSpecularity()
{
	return float( (material_roughness_specularity_existMapFlag & 0x00ff0000) >> 16 ) / 255.0f;
}

float GetMaterialRoughness()
{
	return float( (material_roughness_specularity_existMapFlag & 0xff000000) >> 24 ) / 255.0f;
}

uint GetMaterialExistMapFlag()
{
	return material_roughness_specularity_existMapFlag & 0x0000ffff;
}

uint GetMaterialFlag()
{
	return material_flag;
}

bool HasDiffuseMap()	{	return (GetMaterialExistMapFlag() & (1 << 0));	}
bool HasNormalMap()	{	return (GetMaterialExistMapFlag() & (1 << 1));	}
bool HasOpacityMap()	{	return (GetMaterialExistMapFlag() & (1 << 2));	}
bool HasHeightMap()	{	return (GetMaterialExistMapFlag() & (1 << 3));	}
bool HasMetallicMap()	{	return (GetMaterialExistMapFlag() & (1 << 4));	}
bool HasOcclusionMap()	{	return (GetMaterialExistMapFlag() & (1 << 5));	}
bool HasRoughnessMap()	{	return (GetMaterialExistMapFlag() & (1 << 6));	}
bool HasEmissionMap()	{	return (GetMaterialExistMapFlag() & (1 << 7));	}

float3 GetAlbedo(SamplerState samplerState, float2 uv)
{
	float3 mtlMainColor	= GetMaterialMainColor().rgb;
	float3 diffuseTex	= DiffuseMap.Sample(samplerState, uv).rgb;

	return lerp(mtlMainColor, diffuseTex * mtlMainColor, HasDiffuseMap());
}

float3 GetOcclusion(SamplerState samplerState, float2 uv)
{
	return OcclusionMap.Sample(samplerState, uv).x;
}

float GetRoughness(SamplerState samplerState, float2 uv)
{
	float roughnessTex = RoughnessMap.Sample(samplerState, uv).x;
	float matRoughness = GetMaterialRoughness();

	return lerp(matRoughness, roughnessTex, HasRoughnessMap());
}

float3 GetEmissiveColor(SamplerState samplerState, float2 uv)
{
	float3 mtlEmissiveColor	= GetMaterialEmissiveColor();
	float3 emissiveTex	= EmissionMap.Sample(samplerState, uv).rgb;

	return lerp(mtlEmissiveColor, emissiveTex * mtlEmissiveColor, HasEmissionMap());
}

float GetMetallic(SamplerState samplerState, float2 uv)
{
	float metallicTex = MetallicMap.Sample(samplerState, uv).x;
	float mtlMetallic = GetMaterialMetallic();

	return lerp(mtlMetallic, metallicTex, HasMetallicMap());
}

float GetHeight(SamplerState samplerState, float2 uv)
{
	return HeightMap.Sample(samplerState, uv).x;
}

void GetDiffuseSpecularColor(out float3 outDiffuseColor, out float3 outSpecularColor, float3 baseColor, float specularity, float metallic)
{
	outDiffuseColor		= baseColor - baseColor * metallic;
	outSpecularColor	= lerp(0.08f * specularity.xxx, baseColor, metallic);
}

float GetAlpha(SamplerState samplerState, float2 uv)
{
	float DiffuseMapAlpha	= lerp(1.0f, DiffuseMap.Sample(samplerState, uv).a, HasDiffuseMap());
	float alpha		= DiffuseMapAlpha * (1.0f - OpacityMap.Sample(samplerState, uv).x) * GetMaterialMainColor().a;

	return alpha;
}

#endif
