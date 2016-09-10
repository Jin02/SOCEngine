//NOT_CREATE_META_DATA

#ifndef __SOC_INJECTION_COMMON_H__
#define __SOC_INJECTION_COMMON_H__

#define NEVER_USE_VSM
#define NOT_USE_SHADOW_STRENGTH
#define USE_OUT_VOXEL_MAP

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "Voxelization_Common.h"
#include "GICommon.h"

#ifdef USE_RAW_BUFFER_VOXELIZATION
Buffer<uint>		VoxelAlbedoMap												: register( t29 );
Buffer<uint>		VoxelEmissionMap											: register( t31 );
Buffer<uint>		VoxelNormalMap												: register( t30 );
#else
Texture3D<float4>	VoxelAlbedoMap												: register( t29 );
Texture3D<float4>	VoxelEmissionMap											: register( t31 );
Texture3D<float4>	VoxelNormalMap												: register( t30 );
#endif

StructuredBuffer<DSLightVPMat>	DirectionalLightShadowInvVPVMatBuffer		: register( t32 );
StructuredBuffer<PLightVPMat>	PointLightShadowInvVPVMatBuffer				: register( t33 );
StructuredBuffer<DSLightVPMat>	SpotLightShadowInvVPVMatBuffer				: register( t34 );

RWTexture3D<uint> OutVoxelColorMap											: register( u0 );

#ifdef USE_RAW_BUFFER_VOXELIZATION
float4 GetColor(Buffer<uint> voxelMap, uint3 voxelIdx, uint cascade)
{
	uint bufferIndex = GetFlattedVoxelIndex(voxelIdx, cascade, uint(GetDimension()));
	return RGBA8UintColorToFloat4(voxelMap.Load(bufferIndex * 4));
}

float3 GetNormal(Buffer<uint> voxelNormalMap, uint3 voxelIdx, uint cascade)
{
	uint bufferIndex = GetFlattedVoxelIndex(voxelIdx, cascade, uint(GetDimension()));
	float3 normal = RGBA8UintColorToFloat4(voxelNormalMap.Load(bufferIndex * 4)).rgb;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}
#else
float4 GetColor(Texture3D<float4> voxelTexture, uint3 voxelIdx, uint cascade)
{
	uint3 idx = voxelIdx;
	idx.y += cascade * uint(GetDimension());

	return voxelTexture.Load(int4(idx, 0));
}

float3 GetNormal(Texture3D<float4> voxelNormalMap, uint3 voxelIdx, uint cascade)
{
	voxelIdx.y += cascade * int(GetDimension());

	float3 normal = voxelNormalMap.Load(int4(voxelIdx, 0)).xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}
#endif

#endif
