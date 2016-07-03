//EMPTY_META_DATA
//#16_0 기반으로 작업 중..

#define USE_VIEW_INFORMATION

#include "FullScreenShader.h"
#include "RayTracingCommon.h"
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

float4 SampleWeightedColorUsingCone(float2 samplePos, float lod, float gloss)
{
	float3 color = unknown_colorBuffer.SampleLevel(trilinearClampSampler, samplePos, lod).rgb;
	return float4(color * gloss, gloss);
}


float4 SSCT_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	int3 screenPos				= int3(input.position.xy, 0);
	
	float4 screenSpaceRay		= unknown_rayTracingBuffer.Load(screenPos);
	float3 fallbackColor		= unknown_indirectSpecularBuffer.Load(screenPos).rgb;
	
	// screenSpaceRay.w is RdotV in SSRT
	// so, We can check no hit or ray faces back towards camera.
	if(screenSpaceRay.w <= 0.0f)
		return float4(fallbackColor, 1.0f);

	
	float	linearDepth			= LinearizeDepth( unknown_depthBuffer.Load(screenPos).r );
	float3	viewRayOrigin		= input.viewRay * linearDepth;
	float3	eyeToRayOriginDir	= normalize(viewRayOrigin);
	

	float4	specularColor		= unknown_specularLightBuffer.Load(screenPos);
	float	roughness			= ??????????????;
	float	specularPower		= SpecularPowerFromRoughness(roughness);
	
	// 전체 콘(Isosceles Triangle) 각도에서 절반만 필요하다.
	float	coneTheta			= ConeAngleFromSpecularPower(specularPower) * 0.5f;

	float3	screenSpacePosition	= float3(input.uv, depth);
	float2	dp					= screenSpaceRay.xy - screenSpacePosition.xy;
	float	adjacentLength		= length(dp);
	float2	adjacentUnit		= normalize(dp);
	
	float4	resultColor			= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float	alpha				= 1.0f;
	float	glossMult			= 1.0f - roughness;
	
	for(uint i=0; i<??; ++i)
	{
		float	oppositeLength	= IsoscelesTriangleOpposite(adjacentLength, coneTheta);
		float	inRadius		= IsoscelesTriangleInRadius(oppositeLength, adjacentLength);
		float2	samplePos		= screenSpacePosition.xy + adjacentUint * (adjacentLength - inRadius);
		
		// 
		float	lod				= clamp( log2(inRadius * max(unknown_depthBufferSize.x, unknown_depthBufferSize.y)),
										0.0f, maxMipLevel);
		float4	newColor		= SampleWeightedColorUsingCone(samplePos, lod, glossMult);
		
		alpha -= newColor.a;
		if(alpha < 0.0f)
			newColor.rgb *= 1.0f - abs(alpha);
		
		resultColor += newColor;
		
		if(resultColor.a >= 1.0f)
			break;
			
		adjacentLength			= IsoscelesTriangleNextAdjacent(adjacentLength, inRadius);
		glossMult 				*= 1.0f - roughness;
	}
	
	//작성중
}
