#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float4 position					: POSITION;
	float2 uv						: TEXCOORD0;
	float3 normal					: NORMAL;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;

	float2 uv						: TEXCOORD0;
	float3 normal 					: NORMAL;
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

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float3 normal	= input.normal;
	return Lighting(normal, input.positionWorld, input.position.xy, input.uv);
}

/******************** Only Position, Only Write DepthBuffer ********************/

PS_POSITION_ONLY_INPUT DepthOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	ps.position = mul(input.position, transform_worldViewProj);

	return ps;
}