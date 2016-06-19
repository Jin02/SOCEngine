#include "ReflectionProbe_Common.h"

struct VS_INPUT
{
	float3 position				: POSITION;
	float3 normal				: NORMAL;
	float3 tangent				: TANGENT;
	float2 uv				: TEXCOORD0;
};

struct GS_REFLECTION_PROBE_INPUT
{
	float3 localPos				: LOCAL_POSITION;
	float3 worldPos				: WORLD_POSITION;
	float3 normal 				: NORMAL;
	float3 tangent				: TANGENT;
	float2 uv				: TEXCOORD0;
};

GS_REFLECTION_PROBE_INPUT VS(VS_INPUT input)
{
	GS_REFLECTION_PROBE_INPUT output = (GS_REFLECTION_PROBE_INPUT)0;

	output.localPos	= input.position;
	output.worldPos	= mul(float4(input.position, 1.0f), transform_world).xyz;

	output.normal	= mul(input.normal, (float3x3)transform_worldInvTranspose);
	output.tangent	= mul(input.tangent, (float3x3)transform_worldInvTranspose);
	output.uv	= input.uv;

	return output;
}

struct PS_REFLECTION_PROBE_INPUT
{
	float4 position 	 		: SV_POSITION;

	float3 localPos				: LOCAL_POS;
	float3 worldPos				: WORLD_POS;
	float3 normal 				: NORMAL;
	float3 tangent				: TANGENT;
	float2 uv				: TEXCOORD0;

	uint rtIndex				: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS(triangle GS_REFLECTION_PROBE_INPUT input[3], inout TriangleStream<PS_REFLECTION_PROBE_INPUT> stream)
{
    for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
		PS_REFLECTION_PROBE_INPUT output;
		output.rtIndex = faceIdx;

		for(uint i=0; i<3; ++i)
		{
			output.position = mul(float4(input[i].worldPos, 1.0f), rpInfo_viewProjs[faceIdx]);

			output.worldPos = input[i].worldPos;
			output.localPos = input[i].localPos;

			output.normal	= input[i].normal;
			output.tangent	= input[i].tangent;
			output.uv	= input[i].uv;

			stream.Append(output);
		}
		stream.RestartStrip();
    }
}

float4 PS(GS_REFLECTION_PROBE_INPUT input) : SV_TARGET
{
	float4 normalMapXYZ	= NormalMap.Sample(defaultSampler, input.uv);
	float3 bumpedNormal	= NormalMapping(normalMapXYZ.rgb, input.normal, input.tangent, input.uv);
	float3 normal		= lerp(normalize(input.normal), bumpedNormal, HasNormalMap());

	return ReflectionProbeLighting(normal, input.worldPos, input.uv);
}

#include "ReflectionProbeAlphaTestWithDiffuse.hlsl"
#include "ReflectionProbeOnlyDepth.hlsl"
