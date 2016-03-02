#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float3 position					: POSITION;
	float3 normal					: NORMAL;
	float2 uv						: TEXCOORD0;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;

	float2 uv						: TEXCOORD0;
	float3 normal 					: NORMAL;
};

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position			= mul(posWorld,						camera_viewProjMat);
	ps.positionWorld	= posWorld.xyz;
	
	ps.uv				= input.uv;
	ps.normal 			= mul(input.normal, (float3x3)transform_worldInvTranspose);

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float3 normal	= normalize(input.normal);
	return Lighting(normal, input.positionWorld, input.position.xy, input.uv);
}

#include "OptionalRendering_Forward.h"