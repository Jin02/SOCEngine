#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float4 position					: POSITION;
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
	ps.positionWorld	= mul(input.position, transform_world).xyz;
	ps.position			= mul(input.position, transform_worldViewProj);
	ps.uv				= input.uv;
	ps.normal			= mul(float4(input.normal, 0), transform_world).xyz;
	ps.tangent			= mul(float4(input.tangent, 0), transform_world).xyz;

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float4 normalMap = normalTexture.Sample(defaultSampler, input.uv);
	bool hasNormalMap = HasNormalTexture();

	float3 bumpedNormal = NormalMapping(normalMap.rgb, input.normal, input.tangent, input.uv);
	float3 normal	= lerp(input.normal, bumpedNormal, hasNormalMap);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalMap.a;
	return Lighting(normal, roughness, input.positionWorld, input.position.xy, input.uv);
#else
	return Lighting(normal, input.positionWorld, input.position.xy, input.uv);
#endif
}

/******************** Only Position, Only Write DepthBuffer ********************/

PS_POSITION_ONLY_INPUT DepthOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	ps.position = mul(input.position, transform_worldViewProj);

	return ps;
}