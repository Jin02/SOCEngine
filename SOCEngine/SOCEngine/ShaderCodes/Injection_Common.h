//NOT_CREATE_META_DATA

#ifndef __SOC_INJECTION_COMMON_H__
#define __SOC_INJECTION_COMMON_H__

#define NEVER_USE_VSM
#define NOT_USE_SHADOW_STRENGTH

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "Voxelization_Common.h"
#include "GICommon.h"

StructuredBuffer<DSLightVPMat>	DirectionalLightShadowInvVPVMatBuffer		: register( t32 );
StructuredBuffer<PLightVPMat>	PointLightShadowInvVPVMatBuffer			: register( t33 );
StructuredBuffer<DSLightVPMat>	SpotLightShadowInvVPVMatBuffer			: register( t34 );

RWTexture3D<uint> OutVoxelColorMap						: register( u0 );

float4 GetColor(Texture3D<float4> voxelMap, uint3 voxelIdx, uint cascade)
{
	uint3 idx = voxelIdx;
	idx.y += cascade * uint(GetDimension());

	return voxelMap.Load(int4(idx, 0));
}

float3 GetNormal(Texture3D<float4> voxelNormalMap, uint3 voxelIdx, uint cascade)
{
	voxelIdx.y += cascade * int(GetDimension());

	float3 normal = voxelNormalMap.Load(int4(voxelIdx, 0)).xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}

#endif
