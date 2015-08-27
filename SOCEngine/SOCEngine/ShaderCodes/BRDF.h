//EMPTY_META_DATA

#ifndef __SOC_BRDF_H__
#define __SOC_BRDF_H__

#include "ShaderCommon.h"

/*
DIFFUSE_LAMBERT
DIFFUSE_BURLEY
DIFFUSE_OREN_NAYAR

DISTRIBUTION_BECKMANN
DISTRIBUTION_BLINN_PHONG
DISTRIBUTION_GGX

GEOMETRY_IMPLICIT
GEOMETRY_NEUMANN
GEOMETRY_COOK_TORRANCE
GEOMETRY_KELEMEN
GEOMETRY_SCHLICK
GEOMETRY_GGX_SMITH

DIFFUSE_ENERGY_CONSERVATION_NONE
DIFFUSE_ENERGY_CONSERVATION_1_MINUS_FRESNEL
DIFFUSE_ENERGY_CONSERVATION_1_MINUS_F0
*/

#define DIFFUSE_BURLEY
#define DISTRIBUTION_GGX
#define GEOMETRY_COOK_TORRANCE
#define DIFFUSE_ENERGY_CONSERVATION_1_MINUS_FRESNEL

float ClampedPow(float x, float y)
{
	return pow( max(abs(x), 0.0000001f), y );
}

// The scattering of electromagnetic waves from rough surfaces
float DistributionBeckmann(float m2, float NdotH)
{
	float NdotH_Sq	= NdotH * NdotH;
	float expResult	= exp( (NdotH_Sq - 1.0f) / (m2 * NdotH_Sq) ); // e^{- \left[ \frac {tan\alpha}{m} \right] ^2}
	float piTerm	= 1.0f / (PI * m2 * NdotH_Sq * NdotH_Sq);

	return piTerm * expResult;
}

// Models of light reflection for computer synthesized pictures
float DistributionBlinnPhong(float m2, float NdotH)
{
	float d = PI * m2;
	float n = 2.0f / m2 - 2.0f;
	return (1.0f / (PI * m2)) * ClampedPow(NdotH, n);
}

// Microfacet models for refraction through rough surfaces
float DistributionGGX(float m2, float NdotH) //Trowbridge Reitz
{
	float NdotH_Sq = NdotH * NdotH;
	float d = NdotH_Sq * (m2 -1) + 1;

	return m2 / ( PI * d * d );
}



float GeometryImplicit(float NdotL, float NdotV)
{
	return NdotL * NdotV;
}

// Compact metallic reflectance models
float GeometryNeumann(float NdotL, float NdotV)
{
	//float d = max(NdotL, NdotV);
	//return NdotL * NdotV / denominator;
	return 1.0f / (4 * max(NdotL, NdotV));
}

// Cook and Torrance 1982, "A Reflectance Model for Computer Graphics"
float GeometryCookTorrance(float NdotH, float NdotV, float VdotH, float NdotL)
{
	float first_term	= 2.0f * NdotH * NdotV / VdotH;
	float second_term	= 2.0f * NdotH * NdotL / VdotH;

	return min(1.0f, min(first_term, second_term));
}

// A microfacet based coupled specular-matte brdf model with importance sampling
float GeometryKelemen(float NdotL, float NdotV, float VdotH)
{
	return rcp( 4.0f * VdotH * VdotH );
}

// Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
float GeometrySchlick(float NdotL, float NdotV, float roughness)
{
	float m = roughness * roughness;
	float k	= m / 2.0f;

	float NoVTerm = NdotV * (1 - k) + k;
	float NoLTerm = NdotL * (1 - k) + k;

	return 0.25f / (NoVTerm * NoLTerm);
}

// Smith 1967, Geometrical shadowing of a random rough surface
float GeometryGGXSmith(float NdotV, float NdotL, float roughness)
{
	float m = roughness;
	float m2 = roughness;

	float NoVTerm = NdotV + sqrt(NdotV * (NdotV - NdotV * m2) + m2);
	float NoLTerm = NdotL + sqrt(NdotL * (NdotL - NdotL * m2) + m2);

	return rcp(NoVTerm * NoLTerm);
}



float FresnelSchlick(float f0, float LdotH)
{
	return f0 + (1.0f - f0) * pow(1.0f - LdotH, 5.0f);
}



float3 DiffuseLambert(float3 diffuseColor)
{
	return diffuseColor * (1.0f / PI);
}

// Burley 2012, Physically-Based Shading at Disney
// Lagrade et al. 2014, Moving Frostbite to Physically Based Rendering
float3 DiffuseBurley(float3 diffuseColor, float roughness, float NdotV, float NdotL, float VdotH)
{
	float fd90		= (0.5f + 2.0f * VdotH * VdotH) * roughness;
	
	float invNdotV	= 1.0f - NdotL;
	float NdotVPow5	= pow(invNdotV, 5.0f);

	float invNdotL	= 1.0f - NdotL;
	float NdotLPow5	= pow(invNdotL, 5.0f);

	float FdV		= 1.0f + (fd90 - 1.0f) * NdotVPow5;
	float FdL		= 1.0f + (fd90 - 1.0f) * NdotLPow5;

	return diffuseColor * (1.0 / PI * FdV * FdL) * ( 1 - 0.3333f * roughness );
}

// Gotanda 2012, Beyond a Simple Physically Based Blinn-Phong Model in Real-Time
float3 DiffuseOrenNayar(float3 diffuseColor, float roughness, float NdotV, float NdotL, float VdotL)
{
	float m = roughness * roughness;
	float m2 = m * m;

	float A = 1.0f - 0.5f * (m2 / (m2 + 0.57f));
	float B = 0.45f * (m2 / (m2 + 0.09f));
	float cosTerm = VdotL - NdotV * NdotL; //cos(r - i)
	B = B * cosTerm * (cosTerm >= 0 ? min(1.0f, NdotL / NdotV) : NdotL);
	
	return diffuseColor / PI * (NdotL * A + B);
}

float3 Diffuse(float3 diffuseColor, float roughness, float NdotV, float NdotL, float VdotH, float VdotL)
{
#if defined(DIFFUSE_LAMBERT)
	return DiffuseLambert(diffuseColor);
#elif defined(DIFFUSE_BURLEY)
	return DiffuseBurley(diffuseColor, roughness, NdotV, NdotL, VdotH);
#elif defined(DIFFUSE_OREN_NAYAR)
	return DiffuseOrenNayer(diffuseColor, roughness, NdotV, NdotL, VdotL);
#endif
}

float Distribution(float roughness, float NdotH)
{
	float m = roughness * roughness;
	float m2 = m * m;

#if defined(DISTRIBUTION_BECKMANN)
	return DistributionBeckmann(m2, NdotH);
#elif defined(DISTRIBUTION_BLINN_PHONG)
	return DistributionBlinnPhong(m2, NdotH);
#elif defined(DISTRIBUTION_GGX)
	return DistributionGGX(m2, NdotH);
#endif
}

float Geometry(float roughness, float NdotH, float NdotV, float NdotL, float VdotH)
{	
#if defined(GEOMETRY_IMPLICIT)
	return GeometryImplicit(NdotL, NdotV);
#elif defined(GEOMETRY_NEUMANN)
	return GeometryNeumann(NdotL, NdotV);
#elif defined(GEOMETRY_COOK_TORRANCE)
	return GeometryCookTorrance(NdotH, NdotV, VdotH, NdotL);
#elif defined(GEOMETRY_KELEMEN)
	return GeometryKelemen(NdotL, NdotV, VdotH);
#elif defined(GEOMETRY_SCHLICK)
	return GeometrySchlick(NdotL, NdotV, roughness);
#elif defined(GEOMETRY_GGX_SMITH)
	return GeometryGGXSmith(NdotV, NdotL, roughness);
#endif
}

float Fresnel(float f0, float LdotH)
{
	//�������� �����ϱ� �������� ���߿� �������� �ϸ� �� �� ����
	return FresnelSchlick(f0, LdotH); //Default
}

float DiffuseEnergyConservation(float f0, float NdotH)
{
#if defined(DIFFUSE_ENERGY_CONSERVATION_NONE)
	return 1.0f;
#elif defined(DIFFUSE_ENERGY_CONSERVATION_1_MINUS_FRESNEL)
	return 1.0f - Fresnel(f0, NdotH);
#elif defined(DIFFUSE_ENERGY_CONSERVATION_1_MINUS_F0)
	return 1.0f - f0;
#endif
}


void BRDFLighting(out float3 resultDiffuseColor, out float3 resultSpecularColor,
				  in LightingParams lightingParams, in LightingCommonParams commonParamas)
{
	float3 halfVector	= normalize(lightingParams.viewDir + commonParamas.lightDir);

	float NdotL			= saturate(dot(lightingParams.normal,	commonParamas.lightDir));
	float NdotH			= saturate(dot(lightingParams.normal,	halfVector));
	float NdotV			= saturate(dot(lightingParams.normal,	lightingParams.viewDir));
	float VdotH			= saturate(dot(lightingParams.viewDir,	halfVector));
	float LdotH			= saturate(dot(commonParamas.lightDir,	halfVector));
	float VdotL			= saturate(dot(lightingParams.viewDir,	commonParamas.lightDir));

	float	Fr = Fresnel(lightingParams.fresnel0, LdotH) * 
		Geometry(lightingParams.roughness, NdotH, NdotV, NdotL, VdotH) * 
		Distribution(lightingParams.roughness, NdotH) 
		/ (4.0f * NdotL * NdotV);

	float diffuseEnergyConservation = DiffuseEnergyConservation(lightingParams.fresnel0, NdotH);
	resultDiffuseColor	= Diffuse(lightingParams.diffuseColor, lightingParams.roughness, NdotV, NdotL, VdotH, VdotL) * commonParamas.lightColor * diffuseEnergyConservation.xxx * commonParamas.lightIntensity.xxx;

	resultSpecularColor	= Fr * NdotL * lightingParams.specularColor * commonParamas.lightColor * commonParamas.lightIntensity.xxx;
}

#endif