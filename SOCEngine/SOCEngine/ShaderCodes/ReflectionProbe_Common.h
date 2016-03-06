//EMPTY_META_DATA

#ifndef __SOC_REFLECTION_PROBE_COMMON_H__
#define __SOC_REFLECTION_PROBE_COMMON_H__

#define TRANSPARENCY_BACK_FACE_WEIGHT 0.5f

#include "PhysicallyBased_Common.h"
#include "DynamicLighting.h"
#include "EnvIBL.h"

cbuffer RPInfo : register( b5 )
{
	matrix	rpInfo_viewProjs[6];

	uint	rpInfo_packedNumOfLights;
	float3	rpInfo_camWorldPos;

	float	rpInfo_range;
	float	rpInfo_near;
	float2	dummy;
};

SamplerState		defaultSampler			: register( s0 );
Texture2D<float2>	preIntegrateEnvBRDFMap	: register( t29 );

float4 Lighting(float3 normal, float3 vtxWorldPos, float2 uv)
{
	float metallic				= GetMetallic(defaultSampler, uv);
	float roughness				= GetRoughness(defaultSampler, uv);
	float specularity			= GetMaterialSpecularity();

	float3 emissiveColor		= GetEmissiveColor(defaultSampler, uv);
	float3 albedo				= GetAlbedo(defaultSampler, uv);

	LightingParams lightParams;

	lightParams.viewDir			= normalize( tbrParam_cameraWorldPosition - vtxWorldPos );
	lightParams.normal			= normal;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= albedo - albedo * metallic;
	lightParams.specularColor	= lerp(0.08f * specularity.xxx, albedo, metallic);

#if defined(RENDER_TRANSPARENCY)
	float3 accumulativeFrontFaceDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeFrontFaceSpecular	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeBackFaceDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeBackFaceSpecular		= float3(0.0f, 0.0f, 0.0f);
#else
	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);
#endif

	uint pointLightCount = GetNumOfPointLight(rpInfo_packedNumOfLights);
	for(uint pointLightIdx=0; pointLightIdx<pointLightCount; ++pointLightIdx)
	{
		lightParams.lightIndex = pointLightIdx;

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

	uint spotLightCount = GetNumOfSpotLight(rpInfo_packedNumOfLights);
	for(uint spotLightIdx=0; spotLightIdx<spotLightCount; ++spotLightIdx)
	{
		lightParams.lightIndex = spotLightIdx;

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

	uint directionalLightCount = GetNumOfDirectionalLight(rpInfo_packedNumOfLights);
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

	float3	result	= front + back;
	float	alpha	= GetAlpha(defaultSampler, uv);
#else
	float3	result = accumulativeDiffuse + accumulativeSpecular;
	float	alpha = 1.0f;
#endif

	return float4(result + emissiveColor, alpha);
}

float3 IBLPass(float2 uv, float3 worldPos, float3 normal)
{
	ApproximateIBLParam param;
	{
		float3 baseColor	= GetAlbedo(defaultSampler, uv);

		float metallic		= GetMetallic(defaultSampler, uv);
		float specularity	= GetMaterialSpecularity();

		GetDiffuseSpecularColor(param.diffuseColor, param.specularColor, baseColor, specularity, metallic);

		param.normal	= normal;
		param.viewDir	= rpInfo_camWorldPos  - worldPos;
		param.roughness = GetRoughness(defaultSampler, uv);
	}
#if defined(RENDER_TRANSPARENCY)
	float3 frontFaceIBL	= ApproximateIBL(preIntegrateEnvBRDFMap, param);

	param.normal = -normal;
	float3 backFaceIBL	= ApproximateIBL(preIntegrateEnvBRDFMap, param) * TRANSPARENCY_BACK_FACE_WEIGHT;
	float3 result = frontFaceIBL + backFaceIBL;
#else
	float3 result = ApproximateIBL(preIntegrateEnvBRDFMap, param);
#endif

	return result;
}

float4 ReflectionProbeLighting(float3 normal, float3 worldPos, float2 uv)
{
	float4 lighting	= Lighting(normal, worldPos, uv);
	float3 ibl		= IBLPass(uv, worldPos, normal);

	return float4(lighting.rgb + ibl, lighting.a);
}

#endif