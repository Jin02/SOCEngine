//EMPTY_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_FORWARD_COMMON_H__
#define __SOC_PHYSICALLY_BASED_FORWARD_COMMON_H__

#define TRANSPARENCY_BACK_FACE_WEIGHT 0.5f

#include "PhysicallyBased_Common.h"
#include "BRDF.h"
#include "LightCullingCommonCS.h"

Buffer<uint> g_perLightIndicesInTile	: register( t13 );
SamplerState defaultSampler				: register( s0 );

void RenderDirectionalLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, 
#endif
					in LightingParams lightingParams)
{
	float4	lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition = lightCenterWithDirZ.xyz;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].w;

		float2	lightParam			= g_inputDirectionalLightParamBuffer[lightingParams.lightIndex];
		commonParams.lightDir		= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

#if defined(RENDER_TRANSPARENCY)
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, lightingParams, commonParams);

		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, lightingParams, commonParams);
#else
		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);
#endif
	}	
}

void RenderPointLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, 
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
	float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightingParams.lightIndex];
	float3 lightCenterWorldPosition	= lightCenterWithRadius.xyz;

	float3 lightDir					= lightCenterWorldPosition - vertexWorldPosition;
	float distanceOfLightAndVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float lightRadius				= lightCenterWithRadius.w;

	if( distanceOfLightAndVertex < lightRadius )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputPointLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputPointLightColorBuffer[lightingParams.lightIndex].w;
		commonParams.lightDir		= lightDir;

		float x = distanceOfLightAndVertex / lightRadius;
		float k = 100.0f - commonParams.lightIntensity; //testing
		float falloff = -(1.0f / k) * (1.0f - (k + 1) / (1.0f + k * x * x) );

#if defined(RENDER_TRANSPARENCY)
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, lightingParams, commonParams);

		resultFrontFaceDiffuseColor		*= falloff;
		resultFrontFaceSpecularColor	*= falloff;

		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, lightingParams, commonParams);		

		resultBackFaceDiffuseColor		*= falloff;
		resultBackFaceSpecularColor		*= falloff;
#else
		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);

		resultDiffuseColor *= falloff;
		resultSpecularColor *= falloff;
#endif
	}
}

void RenderSpotLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, 
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
	float4 spotLightParam	= g_inputSpotLightParamBuffer[lightingParams.lightIndex];
	float3 spotLightDir;
	{
		spotLightDir.xy		= spotLightParam.xy;
		spotLightDir.z		= sqrt(1.0f - spotLightDir.x*spotLightDir.x - spotLightDir.y*spotLightDir.y);	

		bool isDirZMinus	= spotLightParam.w < 0;
		spotLightDir.z		= spotLightDir.z * (1 - (2 * (uint)isDirZMinus));
	}

	float4	lightCenterWithRadius		= g_inputSpotLightTransformBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition	= lightCenterWithRadius.xyz;
	float	lightRadius					= lightCenterWithRadius.w;

	float3	lightRealWorldPosition		= lightCenterWorldPosition - (spotLightDir * lightRadius);
	float3	lightDir					= lightRealWorldPosition - vertexWorldPosition;
	float	distanceOfLightAndVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float	lightCosineConeAngle		= spotLightParam.z;
	float	currentCosineConeAngle		= dot(-lightDir, spotLightDir);


	if( (distanceOfLightAndVertex < lightRadius) && 
		(lightCosineConeAngle < currentCosineConeAngle) )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputSpotLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputSpotLightColorBuffer[lightingParams.lightIndex].w;
		commonParams.lightDir		= lightDir;

		float falloffCoef = abs(spotLightParam.w);
		float x = distanceOfLightAndVertex / lightRadius;
		float falloff = -(1.0f / falloffCoef) * (1.0f - (falloffCoef + 1) / (1.0f + falloffCoef * x * x) );

#if defined(RENDER_TRANSPARENCY)
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, lightingParams, commonParams);

		resultFrontFaceDiffuseColor		*= falloff;
		resultFrontFaceSpecularColor	*= falloff;

		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, lightingParams, commonParams);		

		resultBackFaceDiffuseColor		*= falloff;
		resultBackFaceSpecularColor		*= falloff;
#else
		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);

		resultDiffuseColor *= falloff;
		resultSpecularColor *= falloff;
#endif
	}
}

#if defined(USE_PBR_TEXTURE)
float4 Lighting(float3 normal, float roughnessInTex, float3 vtxWorldPos, float2 SVPosition, float2 uv)
#else
float4 Lighting(float3 normal, float3 vtxWorldPos, float2 SVPosition, float2 uv)
#endif
{
	float4 diffuseTex	= diffuseTexture.Sample(defaultSampler, uv);
	float4 specularTex	= specularTexture.Sample(defaultSampler, uv);

	float metallic, emission, roughness;
	Parse_Metallic_Roughness_Emission(material_alpha_metallic_roughness_emission, metallic, roughness, emission);

#if defined(USE_PBR_TEXTURE)
	metallic = specularTex.a;
	roughness = roughnessInTex;
#endif

	float3 specularColor = lerp(float3(1.f, 1.f, 1.f), specularTex.rgb, HasSpecularTexture());
	float3 diffuseColor  = lerp(float3(1.f, 1.f, 1.f), diffuseTex.rgb * abs(material_mainColor), HasDiffuseTexture());

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

	uint directionalLightCount = GetNumOfDirectionalLight();
	for(uint directionalLightIdx=0; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

#if defined(RENDER_TRANSPARENCY)
		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderDirectionalLight(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			lightParams);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
#else
		float3 diffuse, specular;
		RenderDirectionalLight(diffuse, specular, lightParams);

		accumulativeDiffuse		+= diffuse;
		accumulativeSpecular	+= specular;
#endif
	}

#if defined(RENDER_TRANSPARENCY)
	float3	result = accumulativeFrontFaceDiffuse + accumulativeFrontFaceSpecular + ( TRANSPARENCY_BACK_FACE_WEIGHT * (accumulativeBackFaceDiffuse + accumulativeBackFaceSpecular) );
	float	alpha = diffuseTex.a * opacityTexture.Sample(defaultSampler, uv).x;
#else
	float3 result = accumulativeDiffuse + accumulativeSpecular;
	float alpha = 1.0f;
#endif

	return float4(result, alpha);
}

#endif