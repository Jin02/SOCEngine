//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

cbuffer Voxelization_Info_CB				: register( b5 )
{
	matrix	voxelization_vp_axis[3];
	float4	voxelization_minPos;
};

SamplerState DefaultSampler					: register( s0 );

RWByteAddressBuffer OutVoxelAlbedoMap		: register( u0 );
RWByteAddressBuffer OutVoxelNormalMap		: register( u1 );
RWByteAddressBuffer OutVoxelEmissionMap		: register( u2 );
RWTexture3D<uint> OutInjectionColorMap		: register( u3 );

void ComputeVoxelizationProjPos(out float4 position[3], out float4 worldPos[3],
								float3 inputLocalPos[3])
{
	worldPos[0] = mul(float4(inputLocalPos[0], 1.0f), transform_world);
	worldPos[1] = mul(float4(inputLocalPos[1], 1.0f), transform_world);
	worldPos[2] = mul(float4(inputLocalPos[2], 1.0f), transform_world);

	float3 NdotAxis = abs( normalize( cross(worldPos[1].xyz - worldPos[0].xyz, worldPos[2].xyz - worldPos[0].xyz) ) );

	uint axisIndex = 0;
	if(		NdotAxis.x > max(NdotAxis.y, NdotAxis.z) )	axisIndex = 0;
	else if(NdotAxis.y > max(NdotAxis.x, NdotAxis.z) )	axisIndex = 1;
	else												axisIndex = 2;

	position[0] = mul(worldPos[0], voxelization_vp_axis[axisIndex]);
	position[1] = mul(worldPos[1], voxelization_vp_axis[axisIndex]);
	position[2] = mul(worldPos[2], voxelization_vp_axis[axisIndex]);
}

void StoreVoxelMapAtomicColorAvgUsingRawBuffer(RWByteAddressBuffer voxelMap, uint flattedVoxelIdx,
											   float4 value)
{
	value *= 255.0f;

	uint newValue			= ToUint(value);
	uint prevStoredValue	= 0;
	uint currentStoredValue	= 0;

	uint count = 0;

	[allow_uav_condition]do//[allow_uav_condition]while(true)
	{
		uint address	= flattedVoxelIdx * 4;
		voxelMap.InterlockedCompareExchange(address, prevStoredValue, newValue, currentStoredValue);

		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;

		float4 curFlt4 = ToFloat4(currentStoredValue);
		curFlt4.xyz = (curFlt4.xyz * curFlt4.w);

		float4 reCompute = curFlt4 + value;
		reCompute.xyz /= reCompute.w;

		newValue = ToUint(reCompute);
	}while(++count < 16);
}

void StoreVoxelMap(float4 albedoWithAlpha, float3 normal, int3 voxelIdx)
{
	if(all(0 <= voxelIdx) & all(voxelIdx < int(gi_dimension)))
	{
		uint index = GetFlattedVoxelIndex(voxelIdx, gi_dimension);

		StoreVoxelMapAtomicColorAvgUsingRawBuffer(OutVoxelAlbedoMap,	index,	albedoWithAlpha);

		float3 storeNormal = normal * 0.5f + 0.5f;
		StoreVoxelMapAtomicColorAvgUsingRawBuffer(OutVoxelNormalMap,	index,	float4(storeNormal, 1.0f));
	}
}

void InjectRadianceFromDirectionalLight(int3 voxelIdx, float3 worldPos, float3 albedo, float alpha, float3 normal)
{
	float3 radiosity	= float3(0.0f, 0.0f, 0.0f);
//	float4 shadowColor	= float4(1.0f, 1.0f, 1.0f, 1.0f);

	uint dlCount = GetNumOfDirectionalLight(gi_packedNumfOfLights);

	for(uint index=0; index<dlCount; ++index)
	{
		float3 lightDir			= GetDirectionalLightDir(index);

		float3 lightColor		= DirectionalLightColorBuffer[index].rgb;
		float3 lambert			= albedo.rgb * saturate(dot(normal, lightDir));
		float intensity			= DirectionalLightColorBuffer[index].a * 10.0f;

//		shadowColor = RenderDirectionalLightShadow(index, worldPos, lightDir);

		radiosity += (lambert / PI) * lightColor * intensity;// * shadowColor.rgb;
		radiosity += GetMaterialEmissiveColor().rgb;
	}

	OutInjectionColorMap[voxelIdx] = Float4ColorToUint(float4(radiosity, alpha));
}

void VoxelizationInPSStage(float3 normal, float2 uv, float3 worldPos)
{
	float4 diffuse	= GetDiffuse(DefaultSampler, uv);
	int3 voxelIdx	= ComputeVoxelIdx(voxelization_minPos.xyz, worldPos);

	StoreVoxelMap(diffuse, normal, voxelIdx);
	InjectRadianceFromDirectionalLight(voxelIdx, worldPos, diffuse.rgb, diffuse.a, normal);
}

#endif
