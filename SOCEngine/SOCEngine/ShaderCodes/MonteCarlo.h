//NOT_CREATE_META_DATA

#ifndef __SOC_MONTE_CARLO_H__
#define __SOC_MONTE_CARLO_H__

#include "../GlobalDefine.h"

float RadicalInverse_VdC( uint bits )
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
	bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );

	return float(bits) * 2.3283064365386963e-10;
}

float2 Hammersley(uint index, uint numSamples)
{
	return float2(float(index) / float(numSamples), RadicalInverse_VdC(index));
}

// Unreal4 MonteCarlo.usf 참고
// Used for Cone or Specular
float4 ImportanceSampleGGX( float2 e, float roughness )
{
	float m = roughness * roughness;
	float m2 = m * m;

	float phi = 2 * PI * e.x;
	float cosTheta = sqrt( (1 - e.y) / ( 1 + (m2 - 1) * e.y ) );
	float sinTheta = sqrt( 1 - cosTheta * cosTheta );

	float3 dirInCartesian;
	dirInCartesian.x = sinTheta * cos( phi );
	dirInCartesian.y = sinTheta * sin( phi );
	dirInCartesian.z = cosTheta;
	
	float d = ( cosTheta * m2 - cosTheta ) * cosTheta + 1;
	float D = m2 / ( PI*d*d );
	float PDF = D * cosTheta;

	return float4( dirInCartesian, PDF );
}

// Unreal4 MonteCarlo.usf 참고
// Used For diffuse
float4 CosineSampleHemisphere( float2 e )
{
	float phi = 2 * PI * e.x;
	float cosTheta = sqrt( e.y );
	float sinTheta = sqrt( 1 - cosTheta * cosTheta );

	float3 dirInCartesian;
	dirInCartesian.x = sinTheta * cos( phi );
	dirInCartesian.y = sinTheta * sin( phi );
	dirInCartesian.z = cosTheta;

	float PDF = cosTheta / PI;

	return float4( dirInCartesian, PDF );
}

// 그 곳에서 참고
float3 TangentToWorld( float3 vec, float3 tangentZ )
{
	float3 upVector = abs(tangentZ.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangentX = normalize( cross( upVector, tangentZ ) );
	float3 tangentY = cross( tangentZ, tangentX );
	return tangentX * vec.x + tangentY * vec.y + tangentZ * vec.z;
}

#endif