//EMPTY_META_DATA

#define NOT_USE_BRDF_LIGHTING

#include "MonteCarlo.h"
#include "BRDF.h"

RWTexture2D<float2> OutMap : register( u0 );

float2 IntegrateBRDF(float Roughness, float NoV, uniform uint sampleCount)
{
	float3 V;
	V.x = sqrt(1.0f - NoV * NoV); // sin
	V.y = 0;
	V.z = NoV; // cos
	float A = 0;
	float B = 0;

	const uint NumSamples = sampleCount;

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples, uint2(0, 0));
		float3 H = ImportanceSampleGGX(Xi.xy, Roughness).xyz;
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));
		if (NoL > 0)
		{
			float G = GeometrySmith(NoV, NoL, Roughness);
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow(1 - VoH, 5);
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	return float2(A, B) / NumSamples;
}

[numthreads(PRE_INTEGRATE_TILE_RES, PRE_INTEGRATE_TILE_RES, 1)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	uint2 size;
	OutMap.GetDimensions(size.x, size.y);

	float2 sampleUV		= float2(globalIdx.xy) / float2(size);
	float2 sampledBRDF	= IntegrateBRDF(sampleUV.x, sampleUV.y, BRDF_SAMPLES);

	OutMap[sampleUV] = sampledBRDF;
}