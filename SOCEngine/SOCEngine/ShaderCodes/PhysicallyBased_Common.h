//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_COMMON_H__
#define __SOC_PHYSICALLY_BASED_COMMON_H__

#include "NormalMapping.h"
#include "CommonConstBuffer.h"
#include "TBRParam.h"

#if (MSAA_SAMPLES_COUNT > 1)
#define ALPHA_TEST_BIAS		0.003f
#else
#define ALPHA_TEST_BIAS		0.5f
#endif

cbuffer Material : register( b3 )		//PhysicallyBasedMaterial
{
	uint	material_mainColor_alpha;
	uint	material_emissiveColor_Metallic;
	uint	material_roughness_specularity_existTextureFlag;
	uint	material_flag_ior;
};

Texture2D diffuseMap			: register( t8 );
Texture2D normalMap				: register( t9 );
Texture2D opacityMap			: register( t10 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering
Texture2D heightMap				: register( t11 );
Texture2D metallicMap			: register( t12 );
Texture2D occlusionMap			: register( t13 );
Texture2D roughnessMap			: register( t33 );
Texture2D emissionMap			: register( t34 );

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
	return (material_flag_ior >> 8) & 0xf;
}

float GetIndexOfRefraction()
{
	float ior8Bit = float( material_flag_ior & 0xf );
	return ior8Bit * rcp(255.0f);
}

bool HasDiffuseMap()	{	return (GetMaterialExistTextureFlag() & (1 << 0));	}
bool HasNormalMap()		{	return (GetMaterialExistTextureFlag() & (1 << 1));	}
bool HasOpacityMap()	{	return (GetMaterialExistTextureFlag() & (1 << 2));	}
bool HasHeightMap()		{	return (GetMaterialExistTextureFlag() & (1 << 3));	}
bool HasMetallicMap()	{	return (GetMaterialExistTextureFlag() & (1 << 4));	}
bool HasOcclusionMap()	{	return (GetMaterialExistTextureFlag() & (1 << 5));	}
bool HasRoughnessMap()	{	return (GetMaterialExistTextureFlag() & (1 << 6));	}
bool HasEmissionMap()	{	return (GetMaterialExistTextureFlag() & (1 << 7));	}

float4 GetDiffuse(SamplerState samplerState, float2 uv)
{
	float4 mtlMainColor	= GetMaterialMainColor();
	float4 diffuseTex	= diffuseMap.Sample(samplerState, uv);

	return HasDiffuseMap() ? diffuseTex * mtlMainColor : mtlMainColor;
}

float GetOcclusion(SamplerState samplerState, float2 uv)
{
	return occlusionMap.Sample(samplerState, uv).x;
}

float3 UnpackNormalMap(SamplerState samplerState, float2 uv, float3 vertexNormal, float3 vertexTangent)
{
	float4 normalTex		= normalMap.Sample(samplerState, uv);
	float3 bumpedNormal		= UnpackNormal(normalTex.rgb, vertexNormal, vertexTangent);
	
	return normalize(lerp(vertexNormal, bumpedNormal, HasNormalMap()));
}

float3 UnpackNormalMapWithoutTangent(SamplerState samplerState, float2 uv, float3 vertexNormal, float3 viewDir)
{
	float3 dp1 = ddx(-viewDir);
	float3 dp2 = ddy(-viewDir);
	float2 duv1 = ddx(uv);
	float2 duv2 = ddy(-uv);

	float3 dp2perp = cross(dp2, vertexNormal);
	float3 dp1perp = cross(vertexNormal, dp1);
	float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	float invmax = rsqrt(max(dot(T, T), dot(B, B)));
	float3x3 tbn = float3x3(T * invmax, B * invmax, vertexNormal);

	float3 map = normalMap.Sample(samplerState, uv).xyz * 2.0f - 1.0f;
	return normalize(mul(map, tbn));
}

float GetRoughness(SamplerState samplerState, float2 uv)
{
	float roughnessTex = roughnessMap.Sample(samplerState, uv).x;
	float matRoughness = GetMaterialRoughness();

	return lerp(matRoughness, roughnessTex, HasRoughnessMap());
}

float3 GetEmissiveColor(SamplerState samplerState, float2 uv)
{
	float3 mtlEmissiveColor	= GetMaterialEmissiveColor();
	float3 emissiveTex		= emissionMap.Sample(samplerState, uv).rgb;

	return lerp(mtlEmissiveColor, emissiveTex * mtlEmissiveColor, HasEmissionMap());
}

float GetMetallic(SamplerState samplerState, float2 uv)
{
	float metallicTex = metallicMap.Sample(samplerState, uv).x;
	float mtlMetallic = GetMaterialMetallic();

	return lerp(mtlMetallic, metallicTex, HasMetallicMap());
}

float GetHeight(SamplerState samplerState, float2 uv)
{
	return heightMap.Sample(samplerState, uv).x;
}

void GetDiffuseSpecularColor(out float3 outDiffuseColor, out float3 outSpecularColor, float3 baseColor, float specularity, float metallic)
{
	outDiffuseColor		= baseColor - baseColor * metallic;
	outSpecularColor	= lerp(0.08f * specularity.xxx, baseColor, metallic);
}

#endif
