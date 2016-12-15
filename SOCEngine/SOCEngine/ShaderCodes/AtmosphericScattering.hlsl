#include "TBDRInput.h"
#include "CommonConstBuffer.h"
#include "ShaderCommon.h"
#include "ToneMapping.h"

cbuffer SkyScatteringParam : register(b1)
{
	uint ssParam_rayleighWithTurbidity;
	uint ssParam_directionalLightIndexWithLuminance;

	float ssParam_mieCoefficient;
	float ssParam_mieDirectionalG;
};

cbuffer SSTransform : register(b3)
{
	matrix ssParam_worldMat;
	matrix ssParam_worldViewProjMat;
};

float GetRayleigh()
{
	return f16tof32(ssParam_rayleighWithTurbidity & 0xffff);
}
float GetTurbidity()
{
	return f16tof32(ssParam_rayleighWithTurbidity >> 16);
}
uint GetDirectionalLightIndex()
{
	return ssParam_directionalLightIndexWithLuminance & 0xffff;
}
float GetLuminance()
{
	return f16tof32(ssParam_directionalLightIndexWithLuminance >> 16);
}
float GetMieCoefficient()
{
	return ssParam_mieCoefficient;
}
float GetMieDirectionalG()
{
	return ssParam_mieDirectionalG;
}


float ComputeSunItensity(float zenithAngleCos)
{
	// earth shadow hack
	const float cutoffAngle	= PI / 1.95f;
	const float steepness	= 1.5f;
	const float EE			= 1000.0f;
	
	// constants for atmospheric scattering
	const float e			= 2.7182818284f;

	zenithAngleCos			= clamp(zenithAngleCos, -1.0f, 1.0f);

	float n = -(cutoffAngle - acos(zenithAngleCos)) / steepness;
	return EE * max(0.0, 1.0f - pow(e, n));
}

float3 ComputeTotalMie(float3 lambda, float T)
{
	// mie stuff
	// K coefficient for the primaries
	const float v	= 4.0f;
	const float3 K	= float3(0.686f, 0.678f, 0.666f);

	float c = (0.2f * T ) * 10E-18f;
	return 0.434f * c * PI * pow((2.0f * PI) / lambda, v - 2.0f) * K;
}

float3 GetLightDir()
{
	uint lightIndex = GetDirectionalLightIndex();
	float3 lightDir = float3(DirectionalLightDirXYBuffer[lightIndex], 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y) * GetSignDirectionalLightDirZSign(DirectionalLightOptionalParamIndex[lightIndex]);

	return lightDir;
}

struct VS_INPUT
{
	float3 position 			: POSITION;
};

struct SS_PS_INPUT
{
	float4 position 			: SV_POSITION;
	float3 worldPos				: WORLD_POS;

	float sunIntensity			: SUN_INTENSITY;
	float sunFade				: SUN_FADE;
};

SS_PS_INPUT VS(VS_INPUT input)
{
	SS_PS_INPUT ps = (SS_PS_INPUT)0;
	float4 worldPos = mul(float4(input.position, 1.0f), ssParam_worldMat);

	ps.position = mul(float4(input.position, 1.0f), ssParam_worldViewProjMat);
	ps.worldPos = worldPos.xyz / worldPos.w;
	
	float3 lightDir	= GetLightDir();
	float camFar	= GetCameraFar();

	float3 sunWorldPos = float3(0.0f, 0.0f, 0.0f);
	sunWorldPos.x = camera_worldPos.x - (lightDir.x * camFar);
	sunWorldPos.y = camera_worldPos.y - (lightDir.y * camFar);
	sunWorldPos.z = camera_worldPos.z - (lightDir.z * camFar);

	float intensity = ComputeSunItensity( dot(lightDir, float3(0.0f, 1.0f, 0.0f)) );
	float fade		= 1.0f - saturate( 1.0f - exp(sunWorldPos.y / camFar ) );
	
	ps.sunFade			= fade;
	ps.sunIntensity		= intensity;

	return ps;
}

float rayleighPhase(float cosTheta)
{
	return (3.0f / (16.0f * PI)) * (1.0f + pow(cosTheta, 2.0f));
}

float hgPhase(float cosTheta, float g)
{
	return (1.0f / (4.0f * PI)) * ((1.0f - pow(g, 2.0f)) / pow(1.0f - 2.0f * g * cosTheta + pow(g, 2.0f), 1.5f));
}

float4 PS(SS_PS_INPUT input) : SV_Target
{		
	float rayleighCoefficient = GetRayleigh() - (1.0f * (1.0f - input.sunFade));
	// wavelength of used primaries, according to preetham
	const float3 lambda = float3(680E-9f, 550E-9f, 450E-9f);

	// mie coefficients
	float3 betaM = ComputeTotalMie(lambda, GetTurbidity()) * GetMieCoefficient();

	// see http://blenderartists.org/forum/showthread.php?321110-Shaders-and-Skybox-madness
	// A simplied version of the total Reayleigh scattering to works on browsers that use ANGLE
	const float3 simplifiedRayleigh = 0.0005f / float3(94, 40, 18);

	// extinction (absorbtion + out scattering)
	// rayleigh coefficients
	float3 betaR = simplifiedRayleigh * rayleighCoefficient;

	const float n = 1.0003f; // refractive index of air
	const float N = 2.545E25f; // number of molecules per unit volume for air at

	// optical length at zenith for molecules
	const float rayleighZenithLength	= 8.4E3f;
	const float mieZenithLength			= 1.25E3f;

	const float sunAngularDiameterCos	= 0.9999f;

	// Scattering
	float3 result = float3(0.0f, 0.0f, 0.0f);
	{
		float3 ToCam		= input.worldPos - camera_worldPos;
		float3 ToCamDir		= normalize(ToCam);
		const float3 up		= float3(0.0f, 1.0f, 0.0f);

		// optical length
		// cutoff angle at 90 to avoid singularity in next formula.
		float zenithAngle	= acos( max(0.0, dot(up, ToCamDir)) );
		float norm			= cos(zenithAngle) + 0.15f * pow(93.885f - RAD_2_DEG(zenithAngle), -1.253f);
		float sR			= rayleighZenithLength	/ norm;
		float sM			= mieZenithLength		/ norm;

		float3 lightDir		= GetLightDir();

		// in scattering
		float cosTheta		= dot(ToCamDir, lightDir);

		float rPhase		= rayleighPhase(cosTheta * 0.5f + 0.5f);
		float3 betaRTheta	= betaR * rPhase;

		float mPhase		= hgPhase(cosTheta, GetMieDirectionalG());
		float3 betaMTheta	= betaM * mPhase;

		// combined extinction factor
		float3 Fex			= exp( -(betaR * sR + betaM * sM) );
		float3 x			= ((betaRTheta + betaMTheta) / (betaR + betaM));

		float3	Lin			=	pow(input.sunIntensity * x * (1.0f - Fex), 1.5f);
				Lin			*=	lerp(float3(1.0f, 1.0f, 1.0f), pow(input.sunIntensity * x * Fex, 1.0f / 2.0f), saturate( pow(1.0f - dot(up, lightDir), 5.0f) ));

		//nightsky
		float3 direction	= ToCamDir;
		float theta			= acos(direction.y); // elevation --> y-axis, [-PI/2, PI/2],
		float phi			= atan2(direction.z, direction.x); // azimuth --> x-axis [-PI/2, PI/2],
		float2 uv			= float2(phi, theta) / float2(2.0f * PI, PI) + float2(0.5f, 0.0f);
		float3 L0			= Fex * 0.1f;

		// composition + solar disc
		float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.00002f, cosTheta);
		L0 += (input.sunIntensity * GetCameraFar() / 2.0f * Fex) * sundisk;

		float3 texColor = (Lin + L0) * 0.04f + float3(0.0f, 0.0003f, 0.00075f);

		float3 curr				= ToLinear(Uncharted2ToneMapping((log2(2.0f / pow(GetLuminance(), 4.0f))) * texColor), tbrParam_gamma);
		const float whiteScale	= 1.0748724675633854f;
		float3 color			= curr * whiteScale;

		float coff = 1.0f / (1.2f + (1.2f * input.sunFade));
		result.rgb = pow(color, coff);
	}

	return float4(ToGamma(result.rgb, tbrParam_gamma), 1.0f);
}
