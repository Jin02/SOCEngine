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

Buffer<uint>	VoxelAlbedoMap												: register( t29 );
Buffer<uint>	VoxelEmissionMap											: register( t31 );
Buffer<uint>	VoxelNormalMap												: register( t30 );

StructuredBuffer<DSLightVPMat>	DirectionalLightShadowInvVPVMatBuffer		: register( t32 );
StructuredBuffer<PLightVPMat>	PointLightShadowInvVPVMatBuffer				: register( t33 );
StructuredBuffer<DSLightVPMat>	SpotLightShadowInvVPVMatBuffer				: register( t34 );

RWTexture3D<uint> OutVoxelColorMap											: register( u0 );

float4 GetColor(Buffer<uint> voxelMap, uint3 voxelIdx, uint cascade)
{
	uint3 idx = voxelIdx;
	idx.y += cascade * uint(GetDimension());

	uint bufferIndex = (idx.x << 2) + (idx.y << 1) + idx.z;
	return RGBA8UintColorToFloat4(voxelMap.Load(bufferIndex));
}

float3 GetNormal(Buffer<uint> voxelNormalMap, uint3 voxelIdx, uint cascade)
{
	voxelIdx.y += cascade * int(GetDimension());

	uint bufferIndex = (voxelIdx.x << 2) + (voxelIdx.y << 1) + voxelIdx.z;
	float3 normal = RGBA8UintColorToFloat4(voxelNormalMap.Load(bufferIndex)).rgb;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}

#endif