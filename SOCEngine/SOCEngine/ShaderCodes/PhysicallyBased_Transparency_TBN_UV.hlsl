#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float4 position					: POSITION;
	float2 uv						: TEXCOORD0;
	float3 normal					: NORMAL;
	float3 tangent					: TANGENT;
	float3 binormal					: BINORMAL;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;

	float2 uv						: TEXCOORD0;

	float3 normal 					: NORMAL;
	float3 tangent 					: TANGENT;
	float3 binormal 				: BINORMAL;
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
	ps.binormal			= mul(float4(input.binormal, 0), transform_world).xyz;

	return ps;
}

float3 DecodeNormal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
	float3 texNormal = DecodeNormalTexture(normalTexture, uv, defaultSampler);
	float3x3 TBN = float3x3(normalize(binormal), normalize(tangent), normalize(normal));

	return normalize( mul(texNormal, TBN) );
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float3 normal	= DecodeNormal(input.normal, input.tangent, input.binormal, input.uv);
	return Lighting(normal, input.positionWorld, input.position.xy, input.uv);
}

/******************** Only Position, Only Write DepthBuffer ********************/

PS_POSITION_ONLY_INPUT DepthOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	ps.position = mul(input.position, transform_worldViewProj);

	return ps;
}