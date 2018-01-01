//EMPTY_META_DATA

#ifndef __SOC_BRDF_H__
#define __SOC_BRDF_H__

#include "ShaderCommon.h"
#include "MonteCarlo.h"

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
GEOMETRY_SMITH
GEOMETRY_SMITH_JOINT_APPROXIMATELY

DIFFUSE_ENERGY_CONSERVATION_NONE
DIFFUSE_ENERGY_CONSERVATION_1_MINUS_FRESNEL
DIFFUSE_ENERGY_CONSERVATION_1_MINUS_F0
*/

#define DIFFUSE_OREN_NAYAR
#define DISTRIBUTION_GGX
#define GEOMETRY_SCHLICK
#define DIFFUSE_ENERGY_CONSERVATION_1_MINUS_FRESNEL

// The scattering of electromagnetic waves from rough surfaces
float DistributionBeckmann(float roughness, float NdotH)
{
	float a = roughness * roughness;
	float a2 = a * a;

	float NdotH_Sq = NdotH * NdotH;
	float expTerm	= exp( (NdotH_Sq - 1.0f) / (a2 * NdotH_Sq) ); // e^{- \left[ \frac {tan\alpha}{a2} \right] ^2}
	float piTerm	= (PI * a2 * NdotH_Sq * NdotH_Sq);

	return expTerm / piTerm;
}

// Models of light reflection for computer synthesized pictures
float DistributionBlinnPhong(float roughness, float NdotH)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float n = 2.0f / a2 - 2.0f;
	return (n + 2.0f) * rcp(2.0f * PI) * pow(max(NdotH, 0.0f), n);
}

// Microfacet models for refraction through rough surfaces
float DistributionGGX(float roughness, float NdotH) //Trowbridge Reitz
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = (NdotH * a2 - NdotH) * NdotH + 1;
	return a2 / (PI * d * d);
}



float GeometryImplicit(float NdotL, float NdotV)
{
	return NdotL * NdotV;
}

// Compact metallic reflectance models
float GeometryNeumann(float NdotL, float NdotV)
{
	float d = max(NdotL, NdotV);
	return NdotL * NdotV / d;
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
	return NdotL * NdotV / (VdotH * VdotH);
}

// Smith와 동작이 일치하도록 조정 됨
// Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
float GeometrySchlick(float NdotL, float NdotV, float roughness)
{
	float k	= (roughness * roughness) * 0.5f;

	float NoVTerm = NdotV * (1.0f - k) + k;
	float NoLTerm = NdotL * (1.0f - k) + k;

	return (NdotL * NdotV) / (NoVTerm * NoLTerm);
}

float GGX(float NdotV, float a)
{
	float k = a * 0.5f;
	return NdotV / (NdotV * (1.0f - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float a = (roughness * roughness);
	return GGX(NdotV, a) * GGX(NdotL, a);
}

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float GeometrySmithJointApproximately(float NdotV, float NdotL, float roughness)
{
	float a = roughness * roughness;

	float smithV = NdotL * ( NdotV * ( 1.0f - a ) + a );
	float smithL = NdotV * ( NdotL * ( 1.0f - a ) + a );

	return 0.5 * rcp( smithV + smithL );
}

float3 FresnelSchlick(float3 specularColor, float VdotH)
{
	float fc = pow(1.0f - VdotH, 5.0f);
	return fc + (1 - fc) * specularColor; 
}



float3 DiffuseLambert(float3 diffuseColor)
{
	return diffuseColor * rcp(PI);
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

	return diffuseColor * (rcp(PI) * FdV * FdL);
}

// Gotanda 2012, Beyond a Simple Physically Based Blinn-Phong Model in Real-Time
float3 DiffuseOrenNayar(float3 diffuseColor, float roughness, float NdotV, float NdotL, float VdotH)
{
	float a = roughness * roughness;
	float s = a;// / ( 1.29 + 0.5 * a );
	float s2 = s * s;
	float VoL = 2 * VdotH * VdotH - 1;		// double angle identity
	float Cosri = VoL - NdotV * NdotL;
	float C1 = 1 - 0.5 * s2 / (s2 + 0.33);
	float C2 = 0.45 * s2 / (s2 + 0.09) * Cosri * (Cosri >= 0 ? rcp(max(NdotL, NdotV)) : 1);
	return diffuseColor * rcp(PI) * (C1 + C2) * (1.0f + roughness * 0.5f); 	
}



float3 Diffuse(float3 diffuseColor, float roughness, float NdotV, float NdotL, float VdotH, float VdotL)
{
#if defined(DIFFUSE_LAMBERT)
	return DiffuseLambert(diffuseColor);
#elif defined(DIFFUSE_BURLEY)
	return DiffuseBurley(diffuseColor, roughness, NdotV, NdotL, VdotH);
#elif defined(DIFFUSE_OREN_NAYAR)
	return DiffuseOrenNayar(diffuseColor, roughness, NdotV, NdotL, VdotH);
#endif
}

float Distribution(float roughness, float NdotH)
{
#if defined(DISTRIBUTION_BECKMANN)
	return DistributionBeckmann(roughness, NdotH);
#elif defined(DISTRIBUTION_BLINN_PHONG)
	return DistributionBlinnPhong(roughness, NdotH);
#elif defined(DISTRIBUTION_GGX)
	return DistributionGGX(roughness, NdotH);
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
#elif defined(GEOMETRY_SMITH)
	return GeometrySmith(NdotV, NdotL, roughness);
#elif defiend(GEOMETRY_SMITH_JOINT_APPROXIMATELY)
	return GeometrySmithJointApproximately(NdotV, NdotL, roughness);
#endif
}

float3 Fresnel(float3 f0, float VdotH)
{
	//나머지는 구현하기 귀찮으니 나중에 생각날때 하면 될 것 같다
	return FresnelSchlick(f0, VdotH); //Default
}

float3 DiffuseEnergyConservation(float3 f0, float NdotL)
{
#if defined(DIFFUSE_ENERGY_CONSERVATION_NONE)
	return float3(1.0f, 1.0f, 1.0f);
#elif defined(DIFFUSE_ENERGY_CONSERVATION_1_MINUS_FRESNEL)
	return float3(1.0f, 1.0f, 1.0f) - Fresnel(f0, NdotL);
#elif defined(DIFFUSE_ENERGY_CONSERVATION_1_MINUS_F0)
	return float3(1.0f, 1.0f, 1.0f) - f0;
#endif
}

// Unreal4의 ReflectionEnvironmentShared.usf에 있는
// ComputeReflectionCaptureMipFromRoughness 이거임. 그리고, 약간 수정함
float ComputeRoughnessLOD(float roughness, uint mipCount)
{
	float levelFrom1x1 = 1.0f - 1.2f * log2(roughness);
	float mip = (float)mipCount - 1 - levelFrom1x1;

	return max(mip, 0.0f);
}

float2 IntegrateBRDF(float Roughness, float NoV, uniform uint sampleCount, uint2 random)
{
	float3 V = float3(	sqrt(1.0f - NoV * NoV), // sin
						0.0f,
						NoV	);					// cos
	float A = 0;
	float B = 0;

	const uint NumSamples = sampleCount;

	for (uint i = 0; i < sampleCount; i++)
	{
		float2 Xi = Hammersley(i, sampleCount, random);
		float3 H = TangentToWorld(ImportanceSampleGGX(Xi, Roughness).xyz, float3(0.0f, 0.0f, 1.0f));
		float3 L = 2 * dot(V, H) * H - V;

		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));

		if (NoL > 0)
		{
			float G = GeometrySmith(NoV, NoL, Roughness);
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow(1.0f - VoH, 5.0f);
			A += (1.0f - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	return float2(A, B) / float(sampleCount);
}


#endif
