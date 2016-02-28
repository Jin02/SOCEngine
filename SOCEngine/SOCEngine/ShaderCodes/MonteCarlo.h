//NOT_CREATE_META_DATA

#ifndef __SOC_MONTE_CARLO_H__
#define __SOC_MONTE_CARLO_H__

#include "../GlobalDefine.h"

uint ReverseBits32( uint bits )
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
	bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
	bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
	bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );

	return bits;
}

// GPU GEM3 - Chapter 20.3과 Unreal4 MonteCarlo.usf, http://cg.informatik.uni-freiburg.de/course_notes/graphics2_04_sampling.pdf 참고
// Hammersley는 의사 난수를 잘 분포시키는 방법 중 하나이다.
// random은 seed라고 생각하면 될듯? 0,0 넣으면 구글에서 검색하면 나오는 표준 테이블 값과 일치하게 뜬다.
// 아래 코드는 p를 2로 고정해서 사용 중.
float2 Hammersley( uint index, uint numSamples, uint2 random )
{
	float idxDivideByNumSample = frac( (float)index / numSamples + float( random.x & 0xffff ) / (1<<16) );
	float res = float( ReverseBits32(index) ^ random.y ) * 2.3283064365386963e-10;

	return float2( idxDivideByNumSample, res );
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