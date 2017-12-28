//EMPTY_META_DATA

#ifndef __SOC_DYNAMIC_LIGHTING_H__
#define __SOC_DYNAMIC_LIGHTING_H__

#include "Shadow.h"
#include "BRDF.h"

struct LightingParams
{
	uint	lightIndex;
	float3	viewDir;
	float3	normal;
	float	roughness;
	float3	diffuseColor;
	float3	specularColor;
};

struct LightingCommonParams
{
	float3	lightColor;
	float3	lightDir;
};

void BRDFLighting(out float3 resultDiffuseColor, out float3 resultSpecularColor, out float outNdotL,
	in LightingParams lightingParams, in LightingCommonParams commonParamas)
{
	float3 halfVector	= normalize(lightingParams.viewDir + commonParamas.lightDir);

	float NdotL			= saturate( dot(lightingParams.normal,	commonParamas.lightDir) );
	float NdotH			= saturate( dot(lightingParams.normal,	halfVector) );
	float NdotV			= saturate( dot(lightingParams.normal,	lightingParams.viewDir) );
	float VdotH			= saturate( dot(lightingParams.viewDir,	halfVector) );
	float VdotL			= saturate( dot(lightingParams.viewDir,	commonParamas.lightDir) );

	float3 fresnel0		= lightingParams.specularColor;
	float roughness		= lightingParams.roughness; //0.6f

	float3 diffuseEnergyConservation = DiffuseEnergyConservation(fresnel0, NdotL);
	float3 diffuseTerm = Diffuse(lightingParams.diffuseColor, roughness, NdotV, NdotL, VdotH, VdotL);
	resultDiffuseColor = diffuseTerm * commonParamas.lightColor * diffuseEnergyConservation;
	resultDiffuseColor = saturate(resultDiffuseColor);

	float3 Fr = ( Fresnel(fresnel0, VdotH) * Geometry(roughness, NdotH, NdotV, NdotL, VdotH) * Distribution(roughness, NdotH) );// / (4.0f * NdotL * NdotV);
	resultSpecularColor	= Fr * commonParamas.lightColor;
	resultSpecularColor	= saturate(resultSpecularColor);

	outNdotL = NdotL;
}

void RenderDirectionalLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
					inout float accumFrontFaceNdotL, inout float accumBackFaceNdotL,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, inout float accumNdotL,
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
#if defined(RENDER_TRANSPARENCY)
	resultFrontFaceDiffuseColor = resultFrontFaceSpecularColor = float3(0, 0, 0);
	resultBackFaceDiffuseColor = resultBackFaceSpecularColor = float3(0, 0, 0);
#else
	resultDiffuseColor	= float3(0, 0, 0);
	resultSpecularColor	= float3(0, 0, 0);
#endif

	uint lightIndex = lightingParams.lightIndex;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= DirectionalLightColorBuffer[lightIndex].xyz;

		float3	lightDir			= GetDirectionalLightDir(lightIndex);;
		commonParams.lightDir		= -normalize(lightDir);

		float intensity = DirectionalLightColorBuffer[lightIndex].a * 5.0f;
#if defined(RENDER_TRANSPARENCY)
		float frontFaceNdotL = 0.0f;
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, frontFaceNdotL, lightingParams, commonParams);
		resultFrontFaceDiffuseColor		*= intensity;
		resultFrontFaceSpecularColor	*= intensity;
		accumFrontFaceNdotL				+= frontFaceNdotL;

		float backFaceNdotL = 0.0f;
		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, backFaceNdotL, lightingParams, commonParams);
		resultBackFaceDiffuseColor		*= intensity;
		resultBackFaceSpecularColor		*= intensity;
		accumBackFaceNdotL				+= backFaceNdotL;
#else
		float NdotL = 0.0f;
		BRDFLighting(resultDiffuseColor, resultSpecularColor, NdotL, lightingParams, commonParams);
		resultDiffuseColor				*= intensity;
		resultSpecularColor				*= intensity;

		accumNdotL						+= NdotL;

		uint shadowIndex = GetShadowIndex(DirectionalLightOptionalParamIndex[lightIndex]);
		if(shadowIndex != -1) //isShadow == true
		{
			float4 shadowColor = RenderDirectionalLightShadow(lightIndex, vertexWorldPosition);

			resultDiffuseColor				*= shadowColor.rgb;
			resultSpecularColor				*= shadowColor.rgb;
		}
#endif
	}	
}

void RenderPointLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
					inout float accumFrontFaceNdotL, inout float accumBackFaceNdotL,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, inout float accumNdotL,
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
#if defined(RENDER_TRANSPARENCY)
	resultFrontFaceDiffuseColor	= resultFrontFaceSpecularColor	= float3(0, 0, 0);
	resultBackFaceDiffuseColor	= resultBackFaceSpecularColor	= float3(0, 0, 0);
#else
	resultDiffuseColor	= float3(0, 0, 0);
	resultSpecularColor	= float3(0, 0, 0);
#endif
	uint lightIndex = lightingParams.lightIndex;
	float4 lightCenterWithRadius	= PointLightTransformBuffer[lightIndex];

	float3 lightCenterWorldPosition	= lightCenterWithRadius.xyz;
	float lightRadius				= lightCenterWithRadius.w;

	float3 lightDir					= lightCenterWorldPosition - vertexWorldPosition;
	float distanceOfLightWithVertex = length(lightDir);
	lightDir = normalize(lightDir);

	if( distanceOfLightWithVertex < lightRadius )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= PointLightColorBuffer[lightIndex].xyz;
		commonParams.lightDir		= lightDir;

		float lumen = PointLightColorBuffer[lightIndex].w * float(MAXIMUM_LUMEN);
		float attenuation = lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);

#if defined(RENDER_TRANSPARENCY)
		float frontFaceNdotL = 0.0f;
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, frontFaceNdotL, lightingParams, commonParams);
		resultFrontFaceDiffuseColor		*= attenuation;
		resultFrontFaceSpecularColor	*= attenuation;
		accumFrontFaceNdotL				+= frontFaceNdotL;

		float backFaceNdotL = 0.0f;
		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, backFaceNdotL, lightingParams, commonParams);		
		resultBackFaceDiffuseColor		*= attenuation;
		resultBackFaceSpecularColor		*= attenuation;
		accumBackFaceNdotL				+= backFaceNdotL;
#else
		float NdotL = 0.0f;
		BRDFLighting(resultDiffuseColor, resultSpecularColor, NdotL, lightingParams, commonParams);

		resultDiffuseColor	*= attenuation;
		resultSpecularColor	*= attenuation;

		accumNdotL			+= NdotL;

		uint shadowIndex = GetShadowIndex(PointLightOptionalParamIndex[lightIndex]);
		if(shadowIndex != -1) //isShadow == true
		{
			float4 shadowColor = RenderPointLightShadow(lightIndex, vertexWorldPosition, lightDir, distanceOfLightWithVertex / lightRadius);

			resultDiffuseColor	*= shadowColor.rgb;
			resultSpecularColor	*= shadowColor.rgb;
		}
#endif
	}
}

void RenderSpotLight(
#if defined(RENDER_TRANSPARENCY)
					out float3 resultFrontFaceDiffuseColor, out float3 resultFrontFaceSpecularColor,
					out float3 resultBackFaceDiffuseColor, out float3 resultBackFaceSpecularColor,
					inout float accumFrontFaceNdotL, inout float accumBackFaceNdotL,
#else
					out float3 resultDiffuseColor, out float3 resultSpecularColor, inout float accumNdotL,
#endif
					in LightingParams lightingParams, float3 vertexWorldPosition)
{
#if defined(RENDER_TRANSPARENCY)
	resultFrontFaceDiffuseColor = resultFrontFaceSpecularColor = float3(0, 0, 0);
	resultBackFaceDiffuseColor = resultBackFaceSpecularColor = float3(0, 0, 0);
#else
	resultDiffuseColor	= float3(0, 0, 0);
	resultSpecularColor	= float3(0, 0, 0);
#endif

	uint lightIndex = lightingParams.lightIndex;

	float4 lightCenterPosWithRadius = SpotLightTransformBuffer[lightIndex];
	float3 lightPos	= lightCenterPosWithRadius.xyz;
	float radiusWithMinusZDirBit = lightCenterPosWithRadius.w;

	float4 spotParam = SpotLightParamBuffer[lightIndex];
	float3 lightDir = -float3(spotParam.x, spotParam.y, 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
	lightDir.z = (radiusWithMinusZDirBit < 0.0f) ? -lightDir.z : lightDir.z;

	float radius = abs(radiusWithMinusZDirBit);

	float outerCosineConeAngle	= spotParam.z;
	float innerCosineConeAngle	= spotParam.w;

	float3 vtxToLight		= lightPos - vertexWorldPosition;
	float3 vtxToLightDir	= normalize(vtxToLight);

	float distanceOfLightWithVertex = length(vtxToLight);

	float currentCosineConeAngle = dot(-vtxToLightDir, lightDir);
	if( (distanceOfLightWithVertex < (radius * 2.0f)) &&
		(outerCosineConeAngle < currentCosineConeAngle) )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= SpotLightColorBuffer[lightIndex].xyz;
		commonParams.lightDir		= vtxToLightDir;

		float innerOuterAttenuation = saturate( (currentCosineConeAngle - outerCosineConeAngle) / (innerCosineConeAngle - outerCosineConeAngle));
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;	// ^2
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;	// ^4
		innerOuterAttenuation = lerp(innerOuterAttenuation, 1, innerCosineConeAngle < currentCosineConeAngle);

		float lumen = SpotLightColorBuffer[lightIndex].w * float(MAXIMUM_LUMEN); //maximum lumen is float(MAXIMUM_LUMEN)

		float plAttenuation = 1.0f / (distanceOfLightWithVertex * distanceOfLightWithVertex);
		float totalAttenTerm = lumen * plAttenuation * innerOuterAttenuation;

#if defined(RENDER_TRANSPARENCY)
		float frontFaceNdotL = 0.0f;
		BRDFLighting(resultFrontFaceDiffuseColor, resultFrontFaceSpecularColor, frontFaceNdotL, lightingParams, commonParams);
		resultFrontFaceDiffuseColor		*= totalAttenTerm;
		resultFrontFaceSpecularColor	*= totalAttenTerm;
		accumFrontFaceNdotL				+= frontFaceNdotL;

		float backFaceNdotL = 0.0f;
		lightingParams.normal = -lightingParams.normal;
		BRDFLighting(resultBackFaceDiffuseColor, resultBackFaceSpecularColor, backFaceNdotL, lightingParams, commonParams);		
		resultBackFaceDiffuseColor		*= totalAttenTerm;
		resultBackFaceSpecularColor		*= totalAttenTerm;
		accumBackFaceNdotL				+= backFaceNdotL;
#else
		float NdotL = 0.0f;
		BRDFLighting(resultDiffuseColor, resultSpecularColor, NdotL, lightingParams, commonParams);

		resultDiffuseColor	*= totalAttenTerm;
		resultSpecularColor	*= totalAttenTerm;
		accumNdotL			+= NdotL;

		uint shadowIndex = GetShadowIndex(SpotLightOptionalParamIndex[lightIndex]);
		if(shadowIndex != -1) //isShadow == true
		{
			float4 shadowColor = RenderSpotLightShadow(lightIndex, vertexWorldPosition, distanceOfLightWithVertex / radius);

			resultDiffuseColor	*= shadowColor.rgb;
			resultSpecularColor	*= shadowColor.rgb;
		}
#endif
	}
}


#endif
