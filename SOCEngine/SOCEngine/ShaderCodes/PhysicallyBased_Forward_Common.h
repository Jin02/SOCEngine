//EMPTY_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_FORWARD_COMMON_H__
#define __SOC_PHYSICALLY_BASED_FORWARD_COMMON_H__

#define TRANSPARENCY_BACK_FACE_WEIGHT 0.5f

#include "PhysicallyBased_Common.h"
#include "LightCullingCommonCS.h"
#include "DynamicLighting.h"

#include "EnvIBL.h"

Buffer<uint> g_perLightIndicesInTile	: register( t27 );
SamplerState defaultSampler				: register( s0 );

float4 Lighting(float3 normal, float3 vtxWorldPos, float2 SVPosition, float2 uv)
{
	float metallic				= GetMetallic(defaultSampler, uv);
	float roughness				= GetRoughness(defaultSampler, uv);
	float specularity			= GetMaterialSpecularity();

	float3 emissiveColor		= ToLinear(GetEmissiveColor(defaultSampler, uv), GetGamma());
	float4 diffuse				= GetDiffuse(defaultSampler, uv);
	float3 albedo				= ToLinear(diffuse.rgb, GetGamma());

	LightingParams lightParams;

	lightParams.viewDir			= normalize( camera_worldPos - vtxWorldPos );
	lightParams.normal			= normal;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= albedo - albedo * metallic;
	lightParams.specularColor	= lerp(0.08f * specularity.xxx, albedo, metallic);

	uint tileIdx				= GetTileIndex(SVPosition);
	uint startIdx				= tileIdx * tbrParam_maxNumOfPerLightInTile + 1;

	uint packedLightCountValue	= g_perLightIndicesInTile[startIdx - 1];
	uint pointLightCount		= packedLightCountValue & 0x0000ffff;
	uint spotLightCount			= packedLightCountValue >> 16;

#if defined(RENDER_TRANSPARENCY)
	float3 accumulativeFrontFaceDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeFrontFaceSpecular	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeBackFaceDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeBackFaceSpecular		= float3(0.0f, 0.0f, 0.0f);

	float accumFrontFaceNdotL	= 0.0f;
	float accumBackFaceNdotL	= 0.0f;
#else
	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);
	float accumNdotL			= 0.0f;
#endif

	uint endIdx = startIdx + pointLightCount;
	for(uint pointLightIdx=startIdx; pointLightIdx<endIdx; ++pointLightIdx)
	{
		lightParams.lightIndex = g_perLightIndicesInTile[pointLightIdx];

#if defined(RENDER_TRANSPARENCY)
		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderPointLight(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			accumFrontFaceNdotL, accumBackFaceNdotL,
			lightParams, vtxWorldPos);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderPointLight(diffuse, specular, accumNdotL, lightParams, vtxWorldPos);

		accumulativeDiffuse		+= diffuse;
		accumulativeSpecular	+= specular;
#endif
	}

	startIdx += pointLightCount;
	endIdx += spotLightCount;
	for(uint spotLightIdx=startIdx; spotLightIdx<endIdx; ++spotLightIdx)
	{
		lightParams.lightIndex = g_perLightIndicesInTile[spotLightIdx];

#if defined(RENDER_TRANSPARENCY)
		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderSpotLight(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			accumFrontFaceNdotL, accumBackFaceNdotL,
			lightParams, vtxWorldPos);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderSpotLight(diffuse, specular, accumNdotL, lightParams, vtxWorldPos);

		accumulativeDiffuse		+= diffuse;
		accumulativeSpecular	+= specular;
#endif
	}

	uint directionalLightCount = GetNumOfDirectionalLight(tbrParam_packedNumOfLights);
	for(uint directionalLightIdx=0; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

#if defined(RENDER_TRANSPARENCY)
		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderDirectionalLight(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			accumFrontFaceNdotL, accumBackFaceNdotL,
			lightParams, vtxWorldPos);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderDirectionalLight(diffuse, specular, accumNdotL, lightParams, vtxWorldPos);

		accumulativeDiffuse		+= diffuse;
		accumulativeSpecular	+= specular;
#endif
	}

	Surface surface;
	surface.normal		= normal;
	surface.albedo		= lightParams.diffuseColor;
	surface.specular	= lightParams.specularColor;
	surface.worldPos	= vtxWorldPos;
	surface.roughness	= roughness;
	surface.depth		= 1.0f; // 어짜피 forward라 IBL에서 depth가 필요없다.

#if defined(RENDER_TRANSPARENCY)
	accumulativeFrontFaceSpecular = saturate(accumulativeFrontFaceSpecular);
	accumulativeBackFaceSpecular = saturate(accumulativeBackFaceSpecular);

	float3 back		= (accumulativeBackFaceDiffuse + accumulativeBackFaceSpecular);
	float3 front	= (accumulativeFrontFaceDiffuse + accumulativeFrontFaceSpecular);

	accumFrontFaceNdotL	= clamp(accumFrontFaceNdotL, 0.2f, 1.0f);
	accumBackFaceNdotL	= clamp(accumBackFaceNdotL, 0.0f, 0.2f);

	float3 iblDiffuse, iblSpecular;
	IBL(iblDiffuse, iblSpecular, surface);	// front face
	front	+= iblDiffuse * accumFrontFaceNdotL;
	back	+= iblSpecular * accumBackFaceNdotL;

	surface.normal = -surface.normal;
	IBL(iblDiffuse, iblSpecular, surface);	// back face
	front	+= iblDiffuse * accumFrontFaceNdotL;
	back	+= iblSpecular * accumBackFaceNdotL;

	float3	result	= front + back * TRANSPARENCY_BACK_FACE_WEIGHT;
	float	alpha	= diffuse.a;
#else
	accumNdotL = clamp(accumNdotL, 0.2f, 1.0f);

	float3 iblDiffuse, iblSpecular;
	IBL(iblDiffuse, iblSpecular, surface);

	float3	result = accumulativeDiffuse + accumulativeSpecular + (iblDiffuse + iblSpecular) * accumNdotL;
	float	alpha = 1.0f;
#endif

	return float4(result + emissiveColor, alpha);
}

#endif
