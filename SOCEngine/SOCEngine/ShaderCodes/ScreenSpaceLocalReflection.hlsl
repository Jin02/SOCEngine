//EMPTY_META_DATA
//#16_0 기반으로 작업 중..

#include "IsoscelesTriangle.h"

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

