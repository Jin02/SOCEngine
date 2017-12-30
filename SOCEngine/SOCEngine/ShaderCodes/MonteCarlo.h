//NOT_CREATE_META_DATA

#ifndef __SOC_MONTE_CARLO_H__
#define __SOC_MONTE_CARLO_H__

#include "../GlobalDefine.h"

uint ReverseBits32(uint bits)
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
	bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );

	return bits;	
}

float2 Hammersley(uint index, uint numSamples, uint2 random)
{
	float E1 = frac( (float(index) / numSamples) + (float(random.x & 0xffff) / (1<<16)) );
	float E2 = float(ReverseBits32(index) ^ random.y) * 2.3283064365386963e-10;

	return float2(E1, E2);
}

// Unreal4 MonteCarlo.usf 참고
// Used for Cone or Specular
float4 ImportanceSampleGGX( float2 e, float roughness )
{
	float m = roughness * roughness;
	float m2 = m * m;

	float phi = 2.0f * PI * e.x;
	float cosTheta = sqrt( (1.0f - e.y) / ( 1.0f + (m2 - 1.0f) * e.y ) );
	float sinTheta = sqrt( 1.0f - cosTheta * cosTheta );

	float3 H;
	H.x = sinTheta * cos( phi );
	H.y = sinTheta * sin( phi );
	H.z = cosTheta;
	
	float d = ( cosTheta * m2 - cosTheta ) * cosTheta + 1.0f;
	float D = m2 / ( PI*d*d );
	float PDF = D * cosTheta;

	return float4( H, PDF );
}

// Unreal4 MonteCarlo.usf 참고
// Used For diffuse
float4 CosineSampleHemisphere( float2 e )
{
	float phi = 2.0f * PI * e.x;
	float cosTheta = sqrt( e.y );
	float sinTheta = sqrt( 1 - cosTheta * cosTheta );

	float3 H;
	H.x = sinTheta * cos( phi );
	H.y = sinTheta * sin( phi );
	H.z = cosTheta;

	float PDF = cosTheta / PI;

	return float4( H, PDF );
}

float3 TangentToWorld( float3 vec, float3 tangentZ )
{
	float3 upVector = abs(tangentZ.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangentX = normalize( cross( upVector, tangentZ ) );
	float3 tangentY = cross( tangentZ, tangentX );
	return tangentX * vec.x + tangentY * vec.y + tangentZ * vec.z;
}

#endif
