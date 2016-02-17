//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

cbuffer Voxelization_Info_CB : register( b5 )
{
	matrix	voxelization_vp_axisX;
	matrix	voxelization_vp_axisY;
	matrix	voxelization_vp_axisZ;

	float3	voxelization_minPos;
	uint	voxelization_currentCascade;
};

SamplerState defaultSampler			: register( s0 );

#if defined(USE_OUT_ANISOTROPIC_VOXEL_TEXTURES)
RWTexture3D<uint> OutVoxelAlbedoTexture		: register( u0 );
RWTexture3D<uint> OutVoxelNormalTexture		: register( u1 );
RWTexture3D<uint> OutVoxelEmissionTexture	: register( u2 );
RWTexture3D<uint> OutInjectionColorMap		: register( u3 );
#endif

void ComputeVoxelizationProjPos(out float4 position[3], out float4 worldPos[3], float3 inputLocalPos[3])
{
	worldPos[0] = mul(float4(inputLocalPos[0], 1.0f), transform_world);
	worldPos[1] = mul(float4(inputLocalPos[1], 1.0f), transform_world);
	worldPos[2] = mul(float4(inputLocalPos[2], 1.0f), transform_world);

	float3 faceNormal = cross(normalize(worldPos[1].xyz - worldPos[0].xyz), normalize(worldPos[2].xyz - worldPos[0].xyz));

	float3 axis;
	axis.x = abs( dot(float3(1, 0, 0), faceNormal) );
	axis.y = abs( dot(float3(0, 1, 0), faceNormal) );
	axis.z = abs( dot(float3(0, 0, 1), faceNormal) );

	matrix viewProjMat;
	if(		axis.x > max(axis.y, axis.z))
		viewProjMat = voxelization_vp_axisX;
	else if(axis.y > max(axis.x, axis.z))
		viewProjMat = voxelization_vp_axisY;
	else if(axis.z > max(axis.x, axis.y))
		viewProjMat = voxelization_vp_axisZ;

	position[0] = mul(worldPos[0], viewProjMat);
	position[1] = mul(worldPos[1], viewProjMat);
	position[2] = mul(worldPos[2], viewProjMat);
}

void ComputeAlbedo(out float3 albedo, out float alpha, float2 uv)
{
	float4 diffuseTex	= diffuseTexture.Sample(defaultSampler, uv);
	float3 mainColor	= abs(material_mainColor);
	albedo				= lerp(mainColor, diffuseTex.rgb * mainColor, HasDiffuseTexture());

	//float opacityMap	= 1.0f - opacityTexture.Sample(defaultSampler, input.uv).x;
	alpha				= 1.0f;//lerp(1.0f, diffuseTex.a, HasDiffuseTexture()) * opacityMap * ParseMaterialAlpha();
}

void StoreVoxelMap(float4 albedoWithAlpha, float3 normal, int3 voxelIdx)
{
#if defined(USE_OUT_ANISOTROPIC_VOXEL_TEXTURES)
	int dimension = int(GetDimension());
	if(all(0 <= voxelIdx) && all(voxelIdx < dimension))
	{
		int3 index = voxelIdx;
		index.y += voxelization_currentCascade * dimension;

		StoreVoxelMapAtomicColorAvg(OutVoxelAlbedoTexture,	index,	albedoWithAlpha, false);

		//StoreVoxelMapAtomicColorAvg(OutVoxelEmissionTexture,	index,	float4(material_emissionColor.xyz, 1.0f));

		float3 storeNormal = normal * 0.5f + 0.5f;
		StoreVoxelMapAtomicColorAvg(OutVoxelNormalTexture,	index,	float4(storeNormal, 1.0f), false);
	}
#endif
}

void ComputeVoxelIdx(out int3 outVoxelIdx, float3 worldPos)
{
	int dimension	= int(GetDimension());

	float voxelSize	= ComputeVoxelSize(voxelization_currentCascade);
	outVoxelIdx		= int3( (worldPos - voxelization_minPos) / voxelSize );
}

void InjectRadianceFromDirectionalLight(int3 voxelIdx, float3 worldPos, float3 albedo, float alpha, float3 normal)
{
#if defined(USE_OUT_ANISOTROPIC_VOXEL_TEXTURES)
	float3 radiosity = float3(0.0f, 0.0f, 0.0f);

	uint dlShadowCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadows);
	for(uint dlShadowIdx=0; dlShadowIdx<dlShadowCount; ++dlShadowIdx)
	{
		ParsedShadowParam shadowParam;
		ParseShadowParam(shadowParam, g_inputDirectionalLightShadowParams[dlShadowIdx]);
		uint lightIndex		= shadowParam.lightIndex;

		float4 lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightIndex];
		float2 lightParam			= g_inputDirectionalLightParamBuffer[lightIndex];
		float3 lightDir				= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		float3 lightColor	= g_inputDirectionalLightColorBuffer[lightIndex].rgb;
		float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
		float intensity		= g_inputDirectionalLightColorBuffer[lightIndex].a * 10.0f;

		radiosity += lambert * lightColor * intensity * RenderDirectionalLightShadow(lightIndex, worldPos);
		radiosity += material_emissionColor.rgb;
	}

	StoreRadiosity(OutInjectionColorMap, radiosity, alpha, normal, voxelIdx, voxelization_currentCascade);
#endif
}

void VoxelizationInPSStage(float3 normal, float2 uv, float3 worldPos)
{
	float	alpha;
	float3	albedo;
	ComputeAlbedo(albedo, alpha, uv);

	int3 voxelIdx;
	ComputeVoxelIdx(voxelIdx, worldPos);

	StoreVoxelMap(float4(albedo, alpha), normal, voxelIdx);
	InjectRadianceFromDirectionalLight(voxelIdx, worldPos, albedo, alpha, normal);
}

#endif