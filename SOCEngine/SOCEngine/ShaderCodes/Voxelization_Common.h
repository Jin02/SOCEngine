//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

cbuffer Voxelization_Info_CB : register( b5 )
{
	matrix	voxelization_vp_axis[3];
//	matrix	voxelization_vp_axisY;
//	matrix	voxelization_vp_axisZ;

	float3	voxelization_minPos;
	uint	voxelization_currentCascade;
};

SamplerState DefaultSampler			: register( s0 );

#if defined(USE_OUT_VOXEL_MAP) || defined(USE_TEXTURE_VOXELIZATION)
RWTexture3D<uint> OutVoxelAlbedoMap			: register( u2 );
RWTexture3D<uint> OutVoxelNormalMap			: register( u3 );
RWTexture3D<uint> OutVoxelEmissionMap		: register( u4 );
#elif defined(USE_RAW_BUFFER_VOXELIZATION)
RWByteAddressBuffer	OutVoxelAlbedoMap		: register( u2 );
RWByteAddressBuffer	OutVoxelNormalMap		: register( u3 );
RWByteAddressBuffer	OutVoxelEmissionMap		: register( u4 );
#endif
RWTexture3D<uint> OutInjectionColorMap		: register( u5 );

#if defined(USE_OUT_VOXEL_MAP) || defined(USE_RAW_BUFFER_VOXELIZATION) || defined(USE_TEXTURE_VOXELIZATION)

void ComputeVoxelizationProjPos(out float4 position[3], out float4 worldPos[3], out uint outAxisIndex,
								float3 inputLocalPos[3])
{
	worldPos[0] = mul(float4(inputLocalPos[0], 1.0f), transform_world);
	worldPos[1] = mul(float4(inputLocalPos[1], 1.0f), transform_world);
	worldPos[2] = mul(float4(inputLocalPos[2], 1.0f), transform_world);

	float3 NdotAxis = abs( normalize( cross(worldPos[1].xyz - worldPos[0].xyz, worldPos[2].xyz - worldPos[0].xyz) ) );

	if(		NdotAxis.x > max(NdotAxis.y, NdotAxis.z) )	outAxisIndex = 0;
	else if(NdotAxis.y > max(NdotAxis.x, NdotAxis.z) )	outAxisIndex = 1;
	else												outAxisIndex = 2;

	position[0] = mul(worldPos[0], voxelization_vp_axis[outAxisIndex]);
	position[1] = mul(worldPos[1], voxelization_vp_axis[outAxisIndex]);
	position[2] = mul(worldPos[2], voxelization_vp_axis[outAxisIndex]);
}

void ComputeAlbedo(out float3 albedo, out float alpha, float2 uv)
{
	float4 diffuseTex	= diffuseMap.Sample(DefaultSampler, uv);
	float3 mainColor	= GetMaterialMainColor().rgb;
	albedo				= lerp(mainColor, diffuseTex.rgb * mainColor, HasDiffuseMap());

	//float opacityMap	= 1.0f - opacityMap.Sample(DefaultSampler, input.uv).x;
	alpha			= 1.0f;//lerp(1.0f, diffuseTex.a, HasDiffuseMap()) * opacityMap * GetMaterialMainColor().a;
}

void StoreVoxelMap(float4 albedoWithAlpha, float3 normal, int3 voxelIdx)
{
	int dimension = int(GetDimension());
	if(all(0 <= voxelIdx) && all(voxelIdx < dimension))
	{
		float3 compressedNormal = normal * 0.5f + 0.5f;

#if defined(USE_OUT_VOXEL_MAP) || defined(USE_TEXTURE_VOXELIZATION)
		int3 index = voxelIdx;
		index.y += voxelization_currentCascade * dimension;

		OutVoxelAlbedoMap[index]		= Float4ColorToUint(albedoWithAlpha);
		OutVoxelEmissionMap[index]		= Float4ColorToUint(float4(GetMaterialEmissiveColor(), 1.0f));
		OutVoxelNormalMap[index]		= Float4ColorToUint(float4(compressedNormal, 1.0f));
#else
		uint index = GetFlattedVoxelIndex(voxelIdx, voxelization_currentCascade, uint(GetDimension()));

		StoreVoxelMapAtomicColorAvgUsingRawBuffer(OutVoxelAlbedoMap,		index,	albedoWithAlpha, false);
		StoreVoxelMapAtomicColorAvgUsingRawBuffer(OutVoxelEmissionMap,		index,	float4(GetMaterialEmissiveColor(), 1.0f), false);
		StoreVoxelMapAtomicColorAvgUsingRawBuffer(OutVoxelNormalMap,		index,	float4(compressedNormal, 1.0f), false);
//		OutVoxelAlbedoMap.Store(index,		Float4ColorToUint(albedoWithAlpha));
//		OutVoxelEmissionMap.Store(index,	Float4ColorToUint(float4(GetMaterialEmissiveColor(), 1.0f)));
//		OutVoxelNormalMap.Store(index,		Float4ColorToUint(float4(compressedNormal, 1.0f)));

#endif
	}
}

void ComputeVoxelIdx(out int3 outVoxelIdx, float3 worldPos)
{
	int dimension	= int(GetDimension());

	float voxelSize	= ComputeVoxelSize(voxelization_currentCascade);
	outVoxelIdx		= int3( (worldPos - voxelization_minPos) / voxelSize );
}

void InjectRadianceFromDirectionalLight(int3 voxelIdx, float3 worldPos, float3 albedo, float alpha, float3 normal)
{
	float3 radiosity = float3(0.0f, 0.0f, 0.0f);

	uint dlShadowCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadows);
	for(uint dlShadowIdx=0; dlShadowIdx<dlShadowCount; ++dlShadowIdx)
	{
		ParsedShadowParam shadowParam;
		ParseShadowParam(shadowParam, DirectionalLightShadowParams[dlShadowIdx]);
		uint lightIndex			= shadowParam.lightIndex;

		float4 lightCenterWithDirZ	= DirectionalLightTransformWithDirZBuffer[lightIndex];
		float2 lightParam		= DirectionalLightParamBuffer[lightIndex];
		float3 lightDir			= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		float3 lightColor		= DirectionalLightColorBuffer[lightIndex].rgb;
		float3 lambert			= albedo.rgb * saturate(dot(normal, lightDir));
		float intensity			= DirectionalLightColorBuffer[lightIndex].a * 10.0f;

		radiosity += lambert * lightColor * intensity;// * RenderDirectionalLightShadow(lightIndex, worldPos);
		radiosity += GetMaterialEmissiveColor().rgb;
	}

	StoreRadiosityUsingRWTexture3D(OutInjectionColorMap, radiosity, alpha, normal, voxelIdx, voxelization_currentCascade);
}

void VoxelizationInPSStage(float3 normal, float2 uv, float3 worldPos, uint axisIndex)
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

#endif
