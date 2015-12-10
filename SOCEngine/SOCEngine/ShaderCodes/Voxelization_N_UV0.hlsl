#define USE_OUT_ANISOTROPIC_VOXEL_TEXTURES

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

	output.worldPos 	= mul( float4(input.position, 1.0f), transform_world ).xyz;
	output.uv			= input.uv;
	output.normal 		= mul(input.normal, (float3x3)transform_world);
 
    return output;
}

struct GS_OUTPUT
{
	float4 position				: SV_POSITION;
	float3 worldPos				: WORLD_POSITION;
	float3 normal				: NORMAL;
	float2 uv					: TEXCOORD0;
};

[instance(1)]
[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
	float3 worldPos0 = input[0].worldPos;
	float3 worldPos1 = input[1].worldPos;
	float3 worldPos2 = input[2].worldPos;

	float3 normal0 = input[0].normal;
	float3 normal1 = input[1].normal;
	float3 normal2 = input[2].normal;

	float3 faceNormal = normalize( cross( (worldPos0 - worldPos1), (worldPos0 - worldPos2) ) );
	float3 vertexNormal = normalize(normal0 + normal1 + normal2);

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


#if defined(USE_VOXELIZATION_BLOATING_POS)
	float3 centerPos = (worldPos0 + worldPos1 + worldPos2) / 3.0f;
	worldPos0 += normalize(worldPos0 - centerPos) * VOXELIZATION_BLOATING_RATIO;
	worldPos1 += normalize(worldPos1 - centerPos) * VOXELIZATION_BLOATING_RATIO;
	worldPos2 += normalize(worldPos2 - centerPos) * VOXELIZATION_BLOATING_RATIO;
#endif

	float4 position0 = mul(float4(worldPos0, 1.0f), viewProjMat);
	float4 position1 = mul(float4(worldPos1, 1.0f), viewProjMat);
	float4 position2 = mul(float4(worldPos2, 1.0f), viewProjMat);

	GS_OUTPUT output;
	output.position	= position0;
	output.worldPos	= input[0].worldPos;
	output.uv		= input[0].uv;
	output.normal	= input[0].normal;
	outputStream.Append(output);

	output.position	= position1;
	output.worldPos	= input[1].worldPos;
	output.uv		= input[1].uv;
	output.normal	= input[1].normal;
	outputStream.Append(output);

	output.position	= position2;
	output.worldPos	= input[2].worldPos;
	output.uv		= input[2].uv;
	output.normal	= input[2].normal;
	outputStream.Append(output);

	outputStream.RestartStrip();
}

void PS( GS_OUTPUT input )
{
	float4 albedo		= diffuseTexture.Sample(defaultSampler, input.uv);
	float opacityMap	= 1.0f - opacityTexture.Sample(defaultSampler, input.uv).x;
	float alpha			= albedo.a * opacityMap * ParseMaterialAlpha();

	float3 normal		= normalize(input.normal);
	
	float3 voxelCoord	= (input.worldPos - voxelization_minPos) / voxelization_voxelizeSize;
	int3 voxelIdx		= ((int3)voxelCoord) * voxelization_dimension;

	float anisotropicNormals[6] = {
		 normal.x,
		-normal.x,
		 normal.y,
		-normal.y,
		 normal.z,
		-normal.z
	};

	voxelIdx.y += (float)voxelization_currentCascade * voxelization_dimension;

	for(int faceIndex=0; faceIndex<6; ++faceIndex)
	{
		voxelIdx.x += (float)faceIndex * voxelization_dimension;

		StoreVoxelMapAtomicColorMax(OutAnistropicVoxelAlbedoTexture, voxelIdx, float4(albedo.xyz * max(anisotropicNormals[faceIndex], 0.0f), alpha));
		StoreVoxelMapAtomicColorMax(OutAnistropicVoxelEmissionTexture, voxelIdx, float4(material_emissionColor.xyz * max(anisotropicNormals[faceIndex], 0.0f), 1.0f));
		StoreVoxelMapAtomicAddNormalOneValue(OutAnistropicVoxelNormalTexture, voxelIdx, max(abs(anisotropicNormals[faceIndex]), 0.0f));
	}
}