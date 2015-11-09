#include "Voxelization_Common.h"

struct VS_INPUT
{
	float3 position 			: POSITION;
	float3 normal				: NORMAL;
	float2 uv					: TEXCOORD0;
};

struct VS_OUTPUT
{
	float3 worldPos				: WORLD_POSITION;
	float3 normal 				: NORMAL;

	float2 uv					: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT output;

	output.worldPos 	= mul( float4(input.position, 1.0f),	transform_world );
	output.uv			= input.uv;
	output.normal 		= mul(input.normal, (float3x3)transform_world);
 
    return output;
}

struct GS_OUTPUT
{
	float4 position				: SV_POSITION;
	float3 worldPos				: WORLD_POSITION;
	float2 uv					: TEXCOORD0;
};

[instance(1)]
[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
	float3 worldPos0 = input[0].worldPos;
	float3 worldPos1 = input[1].worldPos;
	float3 worldPos2 = input[2].worldPos;

	float3 centerPos = (worldPos0 + worldPos1 + worldPos2) / 3.0f;
	worldPos0 += normalize(worldPos0 - centerPos) * VOXELIZATION_BLOATING_RATIO;
	worldPos1 += normalize(worldPos1 - centerPos) * VOXELIZATION_BLOATING_RATIO;
	worldPos2 += normalize(worldPos2 - centerPos) * VOXELIZATION_BLOATING_RATIO;

	float3 normal0 = input[0].normal;
	float3 normal1 = input[1].normal;
	float3 normal2 = input[2].normal;

	float3 faceNormal = normalize( cross( (worldPos0 - worldPos1), (worldPos0 - worldPos2) ) );
	float3 vertexNormal = normalize(normal0 + normal1 + noraml2);

	faceNormal = lerp(faceNormal, -faceNormal, dot(faceNormal, vertexNormal) < 0.0f);

	float3 axis; //주축 선정
	axis.x = abs( dot(float3(1, 0, 0), faceNormal) );
	axis.y = abs( dot(float3(0, 1, 0), faceNormal) );
	axis.z = abs( dot(float3(0, 0, 1), faceNormal) );

	matrix viewProjMat;
	if(		axis.x > max(axis.y, axis.z)) //x가 가장 긴가?
		viewProjMat = voxelization_vp_axisX;
	else if(axis.y > max(axis.x, axis.z)) //y가 ㅁㄴㅇㄹ?
		viewProjMat = voxelization_vp_axisY;
	else if(axis.z > max(axis.x, axis.y)) //zㄱ?
		viewProjMat = voxelization_vp_axisZ;

	float4 position0 = mul(float4(worldPos0, 1.0f), viewProjMat);
	float4 position1 = mul(float4(worldPos1, 1.0f), viewProjMat);
	float4 position2 = mul(float4(worldPos2, 1.0f), viewProjMat);

	GS_OUTPUT output;
	output.position	= position0;
	output.worldPos	= worldPos0;
	output.uv		= input[0].uv;
	outputStream.Append(output);

	output.position	= position1;
	output.worldPos	= worldPos1;
	output.uv		= input[1].uv;
	outputStream.Append(output);

	output.position	= position2;
	output.worldPos	= worldPos2;
	output.uv		= input[2].uv;
	outputStream.Append(output);

	outputStream.RestartStrip();
}

#if defined(VOXELIZATION_DEBUG_MODE)
float4	PS( GS_OUTPUT input ) : SV_Target
#else
void	PS( GS_OUTPUT input )
#endif
{
	float4 albedo = diffuseTexture.Sample(defaultSampler, input.uv);

	uint3 index = (uint3)((input.worldPos - voxelization_minVertexWorldPos) / voxelization_elementSize);
	OutVoxelTexture[index] = D3DX_FLOAT4_to_R8G8B8A8UNORM(albedo);

#if defined(VOXELIZATION_DEBUG_MODE)
	return albedo;
#endif
}