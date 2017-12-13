#define USE_OUT_VOXEL_MAP

#include "Voxelization_Common.h"

struct VS_INPUT
{
	float3 position 		: POSITION;
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float3 localPos			: LOCAL_POSITION;
	float3 normal 			: NORMAL;

	float2 uv				: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT output;

	output.localPos		= input.position;
	output.uv			= input.uv;
	output.normal 		= mul(input.normal, (float3x3)transform_worldInvTranspose);
 
    return output;
}

struct GS_OUTPUT
{
	float4 position				: SV_POSITION;
	float3 worldPos				: WORLD_POSITION;
	float3 normal				: NORMAL;
	float2 uv					: TEXCOORD0;
};

[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
	float3 localPos[3] = 
	{
		input[0].localPos,
		input[1].localPos,
		input[2].localPos,
	};

	uint axisIndex = 0;
	float4 position[3], worldPos[3];
	ComputeVoxelizationProjPos(position, worldPos, localPos);

	[unroll]
	for(uint i=0; i<3; ++i)
	{
		GS_OUTPUT output;
		output.position		= position[i];
		output.uv			= input[i].uv;
		output.normal		= input[i].normal;
		output.worldPos		= worldPos[i].xyz;

		outputStream.Append(output);
	}

	outputStream.RestartStrip();
}

void PS( GS_OUTPUT input )
{
	float3 normal = normalize(input.normal);
	VoxelizationInPSStage(normal, input.uv, input.worldPos);
}