#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float3 position					: POSITION;
	float3 normal					: NORMAL;
	float3 tangent					: TANGENT;
	float2 uv						: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 sv_position 		 	: SV_POSITION;

	float4 position				: POSITION;
	float4 prevPosition			: PREV_POSITION;

	float3 worldPos				: WORLD_POS;
	float3 normal 				: NORMAL;
	float2 uv					: TEXCOORD0;
	float3 tangent 				: TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 localPos		= float4(input.position, 1.0f);
	float4 worldPos		= mul(localPos, transform_world);
	float4 position		= mul(worldPos, camera_viewProjMat);

	output.sv_position 	= position;
	output.worldPos		= worldPos.xyz / worldPos.w;

	output.uv			= input.uv;
	output.normal 		= mul(input.normal, (float3x3)transform_worldInvTranspose);
	output.tangent 		= normalize( mul(input.tangent, (float3x3)transform_worldInvTranspose ) );

	float4 prevWorldPos	= mul(localPos, transform_prevWorld);
	output.prevPosition	= mul(prevWorldPos, camera_prevViewProjMat);

	output.position		= position;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float3 normal = UnpackNormalMap(defaultSampler, input.uv, input.normal, input.tangent);
	return Lighting(normal, input.worldPos, input.position.xy, input.uv);
}

#include "OptionalRendering_Forward.h"