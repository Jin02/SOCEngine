//EMPTY_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_FORWARD_COMMON_H__
#define __SOC_PHYSICALLY_BASED_FORWARD_COMMON_H__

#define TRANSPARENCY_BACK_FACE_WEIGHT 0.5f

#include "PhysicallyBased_Common.h"
#include "LightCullingCommonCS.h"
#include "DynamicLighting.h"

Buffer<uint> g_perLightIndicesInTile	: register( t13 );
SamplerState defaultSampler				: register( s0 );


#if defined(USE_PBR_TEXTURE)
float4 Lighting(float3 normal, float roughnessInTex, float3 vtxWorldPos, float2 SVPosition, float2 uv)
#else
float4 Lighting(float3 normal, float3 vtxWorldPos, float2 SVPosition, float2 uv)
#endif
{
	float4 diffuseTex	= diffuseTexture.Sample(defaultSampler, uv);
	float4 specularTex	= specularTexture.Sample(defaultSampler, uv);

	float metallic, emission, roughness;
	Parse_Metallic_Roughness_Emission(metallic, roughness, emission);

#if defined(USE_PBR_TEXTURE)
	metallic = specularTex.a;
	roughness = roughnessInTex;
#endif

	float3 specularColor = lerp(float3(0.05f, 0.05f, 0.05f), specularTex.rgb, HasSpecularTexture());

	float3 emissionColor	= material_emissionColor.rgb;
	float3 mainColor		= abs(material_mainColor);
	float3 diffuseColor		= lerp(mainColor + emissionColor, diffuseTex.rgb * mainColor + emissionColor, HasDiffuseTexture());

	LightingParams lightParams;

	lightParams.viewDir			= normalize( tbrParam_cameraWorldPosition.xyz - vtxWorldPos );
	lightParams.normal			= normal;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= diffuseColor;
	lightParams.specularColor	= specularColor;

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
#else
	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);
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
			lightParams, vtxWorldPos);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderPointLight(diffuse, specular, lightParams, vtxWorldPos);

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
			lightParams, vtxWorldPos);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderSpotLight(diffuse, specular, lightParams, vtxWorldPos);

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
			lightParams, vtxWorldPos);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderDirectionalLight(diffuse, specular, lightParams, vtxWorldPos);

		accumulativeDiffuse		+= diffuse;
		accumulativeSpecular	+= specular;
#endif
	}

#if defined(RENDER_TRANSPARENCY)
	accumulativeFrontFaceSpecular = saturate(accumulativeFrontFaceSpecular);
	accumulativeBackFaceSpecular = saturate(accumulativeBackFaceSpecular);

	float3 back		= (accumulativeBackFaceDiffuse + accumulativeBackFaceSpecular) * TRANSPARENCY_BACK_FACE_WEIGHT;
	float3 front	= (accumulativeFrontFaceDiffuse + accumulativeFrontFaceSpecular);

	float3	result = front + back;

	float	diffuseTexAlpha = lerp(1.0f, diffuseTex.a, HasDiffuseTexture());
	float	alpha = diffuseTexAlpha * (1.0f - opacityTexture.Sample(defaultSampler, uv).x) * ParseMaterialAlpha();
#else
	float3	result = accumulativeDiffuse + accumulativeSpecular;
	float	alpha = 1.0f;
#endif

	return float4(result, alpha);
}

#endif