//EMPTY_META_DATA

#include "RayTracingCommon.h"
#include "IsoscelesTriangle.h"
#include "BRDF.h"

Texture2D<float4>	InputColorBuffer		: register( t29 );
Texture2D<float4>	RayTracingBuffer		: register( t30 );
Texture2D<float4>	InDirectColorMap		: register( t31 );
Texture2D<float4>	DirectDiffuseColorMap		: register( t32 );
Texture2D<float4>	DirectSpecularColorMap		: register( t33 );


SamplerState		trilinearClampSampler		: register( s0 );

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
	float3 color = InputColorBuffer.SampleLevel(trilinearClampSampler, samplePos, lod).rgb;
	return float4(color * gloss, gloss);
}

float3 ViewSpacePositionFromDepth(float2 uv, float depth)
{
	float3 clipSpacePos = float3(uv, depth) * 2.0f - float3(1.0f, 1.0f, 1.0f);
	float4 viewSpacePos = mul(tbrParam_invProjMat, float4(clipSpacePos, 1.0f));

	return viewSpacePos.xyz / viewSpacePos.w;
}

float4 SSCT_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	int3 screenPos				= int3(input.position.xy, 0);
	
	float4 screenSpaceRay		= RayTracingBuffer.Load(screenPos);
	float3 fallbackColor		= float3(0.04f, 0.04f, 0.04f);//unknown_indirectSpecularBuffer.Load(screenPos).rgb;
	
	// screenSpaceRay.w is RdotV in SSRT
	// so, We can check no hit or ray faces back towards camera.
	if(screenSpaceRay.w <= 0.0f)
		return float4(fallbackColor, 1.0f);

	float	linearDepth		= LinearizeDepth( GBufferDepth.Load(screenPos).r, GetCameraFar() );
	float3	viewSpacePos		= input.viewRay * linearDepth;
	float3	toViewSpacePos		= normalize(viewSpacePos);
	
	float4	specularColor		= DirectSpecularColorMap.Load( screenPos );
	float	roughness			= GBufferNormal_roughness.Load( screenPos ).a;

	float	specularPower		= SpecularPowerFromRoughness(roughness);
	
	// 전체 콘(Isosceles Triangle) 각도에서 절반만 필요하다.
	float	coneTheta			= ConeAngleFromSpecularPower(specularPower) * 0.5f;

	float3	screenSpacePosition	= float3(input.uv, linearDepth);
	float2	dp					= screenSpaceRay.xy - screenSpacePosition.xy;
	float	adjacentLength		= length(dp);
	float2	adjacentUnit		= normalize(dp);
	
	float4	resultColor			= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float	alpha				= 1.0f;
	float	glossMult			= 1.0f - roughness;
	
	float2 depthBufferSize;
	GBufferDepth.GetDimensions(depthBufferSize.x, depthBufferSize.y);

	for(uint i=0; i<14; ++i)
	{
		float	oppositeLength	= IsoscelesTriangleOpposite(adjacentLength, coneTheta);
		float	inRadius		= IsoscelesTriangleInRadius(oppositeLength, adjacentLength);
		float2	samplePos		= screenSpacePosition.xy + adjacentUnit * (adjacentLength - inRadius);
		float	lod				= clamp( log2(inRadius * max(depthBufferSize.x, depthBufferSize.y)),
												0.0f, ssrt_maxMipLevel);
		float4	newColor		= SampleWeightedColorUsingCone(samplePos, lod, glossMult);
		
		alpha -= newColor.a;
		if(alpha < 0.0f)
			newColor.rgb *= 1.0f - abs(alpha);
		
		resultColor += newColor;
		
		if(resultColor.a >= 1.0f)
			break;
			
		adjacentLength			= IsosclesTriangleNextAdjacent(adjacentLength, inRadius);
		glossMult 				*= 1.0f - roughness;
	}
	
	float3 toEye			= -toViewSpacePos;
	float3 worldNormal		= GBufferNormal_roughness.Load(screenPos).rgb * 2.0f - float3(1.0f, 1.0f, 1.0f);
	float3 viewNormal		= mul(float4(worldNormal, 0.0f), camera_viewMat).xyz;
	float3 specular			= FresnelSchlick(specularColor.rgb, abs( dot(viewNormal, toEye) ) );

	float2 boundary			= abs(screenSpaceRay.xy - float2(0.5f, 0.5f)) * 2.0f;
	float fadeDiffRcp		= 1.0f / (ssrt_fadeEnd - ssrt_fadeStart);
	float fadeOnBorder		= 1.0f - saturate((boundary.x - ssrt_fadeStart) * fadeDiffRcp);
	fadeOnBorder			*= 1.0f - saturate((boundary.y - ssrt_fadeStart) * fadeDiffRcp);
	fadeOnBorder			= smoothstep(0.0f, 1.0f, fadeOnBorder);
	float3 rayHitPositionVS		= ViewSpacePositionFromDepth(screenSpaceRay.xy, screenSpaceRay.z);
	float fadeOnDistance		= 1.0f - saturate(distance(rayHitPositionVS, viewSpacePos) / ssrt_maxDistance);
	float fadeOnPerpendicular	= saturate(lerp(0.0f, 1.0f, saturate(screenSpaceRay.w * 4.0f)));
	float fadeOnRoughness		= saturate(lerp(0.0f, 1.0f, (1.0f - roughness) * 4.0f));
	float totalFade				= fadeOnBorder * fadeOnDistance * fadeOnPerpendicular * fadeOnRoughness * (1.0f - saturate(alpha));

	return float4(lerp(fallbackColor, resultColor.rgb * specular, totalFade), 1.0f);
}
