#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float3 position					: POSITION;
	float3 normal					: NORMAL;
	float3 tangent					: TANGENT;
	float2 uv						: TEXCOORD0;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;

	float2 uv						: TEXCOORD0;

	float3 normal 					: NORMAL;
	float3 tangent 					: TANGENT;
};

struct PS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
};

/******************** Render Scene ********************/

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;
	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position 		= mul(posWorld,						camera_viewProj);
	ps.positionWorld	= posWorld.xyz;

	ps.uv				= input.uv;
	ps.normal 			= mul(input.normal, (float3x3)transform_world);
	ps.tangent			= mul(input.tangent, (float3x3)transform_world);

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float4 normalMap = normalTexture.Sample(defaultSampler, input.uv);
	bool hasNormalMap = HasNormalTexture();

	float3 bumpedNormal = NormalMapping(normalMap.rgb, input.normal, input.tangent, input.uv);
	float3 normal = lerp(normalize(input.normal), bumpedNormal, hasNormalMap);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalMap.a;
	return Lighting(normal, roughness, input.positionWorld, input.position.xy, input.uv);
#else
	return Lighting(normal, input.positionWorld, input.position.xy, input.uv);
#endif
}


float4 OnlyAlpaTestWithDiffusePS( PS_SCENE_INPUT input ) : SV_TARGET
{ 
	float4 diffuseTex = diffuseTexture.Sample(defaultSampler, input.uv);
	float opacityMap = 1.0f - opacityTexture.Sample(defaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap * ParseMaterialAlpha();
	if(alpha < ALPHA_TEST_BIAS)
		discard;

	return float4(diffuseTex.rgb, alpha);
}

PS_POSITION_ONLY_INPUT DepthOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position			= mul(posWorld,						camera_viewProj);

	return ps;
}