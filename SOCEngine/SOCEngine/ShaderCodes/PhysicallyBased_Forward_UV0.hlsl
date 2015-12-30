#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float3 position					: POSITION;
	float2 uv						: TEXCOORD0;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;
	float2 uv						: TEXCOORD0;
};

struct PS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
};

/******************** Render Scene ********************/

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;
	float4 posWorld = mul(float4(input.position, 1.0f), transform_world);
	ps.positionWorld = posWorld.xyz;

	ps.position = mul(posWorld, cameraMat_viewProj);

	ps.uv = input.uv;

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float3 normal	= float3(0.0f, 0.0f, 0.0f);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalTexture.Sample(defaultSampler, input.uv).a;
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
	ps.position			= mul(posWorld,						cameraMat_viewProj);

	return ps;
}