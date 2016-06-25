//EMPTY_META_DATA
// #16_0 기반으로 작업 중..

#include "TBDRInput.h"
#include "RayTracing.h"

#define MAX_SPECULAR_EXP 64

float ConeAngleFromSpecularPower(float specularPower)
{
	// based on phong distribution model
	if( specularPower >= exp2(MAX_SPECULAR_EXP) )
		return 0.0f;
	
	const float xi = 0.244f;
	float exponent = 1.0f / (specularPower + 1.0f);
	return acos( pow(xi, exponent) );
}

float SpecularPowerFromRoughness(float roughness)
{
	float a		= roughness * roughness;
	float a2	= a * a;
	
	return (2.0f / a) - 2.0f;
}

// Isosceles Triangles
float IsoscelesTriangleOpposite(float adjacentLength, float coneTheta)
{
	// 간단한 삼각함수 공식들
	// tan(theta) = opp / adj고
	// opp = tan(theta) * adj니까, 그리고 이등변 삼각형이니 2를 곱해주어야 함.

	return 2.0f * tan(coneTheta) * adjacentLength;
}

float IsoscelesTriangleInRadius(float a, float h)
{
	float a2	= a * a;
	float four_h2	= 4.0f * h * h;

	return (a * (sqrt(a2 + four_h2) - a)) / (4.0f * h);
}

float IsosclesTriangleNextAdjacent(float adjLength, float inRadius)
{
	return adjLength - (inRadius * 2.0f);
}
