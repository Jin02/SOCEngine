//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

cbuffer Voxelization_Info_CB				: register( b5 )
{
	matrix	voxelization_vp_axis[3];
#ifdef USE_BLOATING_IN_VOXELIZATION_PASS 
	matrix	voxelization_vp_axis_inv[3];
#endif
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

#ifdef	USE_BLOATING_IN_VOXELIZATION_PASS
	float3 planes[3] =
	{
		cross(position[0].xyw - position[2].xyw, position[2].xyw),
		cross(position[1].xyw - position[0].xyw, position[0].xyw),
		cross(position[2].xyw - position[1].xyw, position[1].xyw),
	};	

	float halfPixel = 1.0f / float(gi_dimension);
	planes[0].z -= dot(halfPixel.xx, abs(planes[0].xy));
	planes[1].z -= dot(halfPixel.xx, abs(planes[1].xy));
	planes[2].z -= dot(halfPixel.xx, abs(planes[2].xy));

	float3 intersection[3] = 
	{
		cross(planes[0], planes[1]),
		cross(planes[1], planes[2]),
		cross(planes[2], planes[0]),
	};

	intersection[0] /= intersection[0].z;
	intersection[1] /= intersection[1].z;
	intersection[2] /= intersection[2].z;
	
	float4 trianglePlane;
	trianglePlane.xyz 	= normalize( cross(position[1].xyz - position[0].xyz, position[2].xyz - position[0].xyz) );
	trianglePlane.w		= -dot(position[0].xyz, trianglePlane.xyz);
	
	float z[3] =
	{
		-(intersection[0].x * trianglePlane.x + intersection[0].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
		-(intersection[1].x * trianglePlane.x + intersection[1].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
		-(intersection[2].x * trianglePlane.x + intersection[2].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;	
	};

	position[0].xyz = vec3(intersection[0].xy, z[0]);
	position[1].xyz = vec3(intersection[1].xy, z[1]);
	position[2].xyz = vec3(intersection[2].xy, z[2]);
	
	[unroll]for(uint i=0; i<3; ++i)
		worldPos[i] = mul(position[i], voxelization_vp_axis_inv[axisIndex]);
#endif	
}

void ComputeAlbedo(out float3 albedo, out float alpha, float2 uv)
{
	float4 diffuseTex	= diffuseMap.Sample(DefaultSampler, uv);
	float3 mainColor	= GetMaterialMainColor().rgb;
	albedo				= lerp(mainColor, diffuseTex.rgb * mainColor, HasDiffuseMap());

	//float opacityMap	= 1.0f - opacityMap.Sample(DefaultSampler, input.uv).x;
	alpha			= 1.0f;//lerp(1.0f, diffuseTex.a, HasDiffuseMap()) * opacityMap * GetMaterialMainColor().a;
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
	if(all(0 <= voxelIdx) && all(voxelIdx < int(gi_dimension)))
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
	float4 shadowColor	= float4(1.0f, 1.0f, 1.0f, 1.0f);

	uint dlShadowCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadows);

	for(uint dlShadowIdx=0; dlShadowIdx<dlShadowCount; ++dlShadowIdx)
	{
		ParsedShadowParam shadowParam;
		ParseShadowParam(shadowParam, DirectionalLightShadowParams[dlShadowIdx]);
		uint lightIndex			= shadowParam.lightIndex;

		float3 lightDir			= float3(DirectionalLightDirXYBuffer[lightIndex], 0.0f);
		lightDir.z				= sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y) * GetSignDirectionalLightDirZSign(DirectionalLightOptionalParamIndex[lightIndex]);

		float3 lightColor		= DirectionalLightColorBuffer[lightIndex].rgb;
		float3 lambert			= albedo.rgb * saturate(dot(normal, lightDir));
		float intensity			= DirectionalLightColorBuffer[lightIndex].a * 10.0f;

		shadowColor = RenderDirectionalLightShadow(lightIndex, worldPos);

		radiosity += lambert * lightColor * intensity * shadowColor.rgb;
		radiosity += GetMaterialEmissiveColor().rgb;
	}

	OutInjectionColorMap[voxelIdx] = Float4ColorToUint(float4(radiosity, alpha));
}

void VoxelizationInPSStage(float3 normal, float2 uv, float3 worldPos)
{
	float	alpha;
	float3	albedo;
	ComputeAlbedo(albedo, alpha, uv);

	int3 voxelIdx = ComputeVoxelIdx(voxelization_minPos.xyz, worldPos);
	StoreVoxelMap(float4(albedo, alpha), normal, voxelIdx);

	InjectRadianceFromDirectionalLight(voxelIdx, worldPos, albedo, alpha, normal);
}

#endif
