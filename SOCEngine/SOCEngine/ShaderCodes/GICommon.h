#ifndef __SOC_GI_COMMON_H__
#define __SOC_GI_COMMON_H__

#include "ShaderCommon.h"

#ifndef VOXEL_CONE_TRACING

Texture3D<float4>	g_inputVoxelAlbedoTexture	: register( t29 );
Texture3D<float4>	g_inputVoxelEmissionTexture	: register( t31 );

#if defined(USE_ANISOTROPIC_VOXELIZATION)
Texture3D<float>	g_inputVoxelNormalTexture	: register( t30 );
#else
Texture3D<float4>	g_inputVoxelNormalTexture	: register( t30 );
#endif

cbuffer GIInfoCB : register( b0 )
#else
cbuffer GIInfoCB : register( b1 )
#endif
{
	// High 16 bit is cascade
	// Low bits is voxel dimension
	uint	gi_maxCascadeWithVoxelDimensionPowOf2;

	float	gi_initVoxelSize;
	float	gi_initWorldSize;
	float	gi_maxMipLevel;
}

uint GetMaximumCascade()
{
	return (gi_maxCascadeWithVoxelDimensionPowOf2 >> 16);
}

uint ComputeCascade(float3 worldPos)
{
	//x, y, z 축 중에 가장 큰걸 고름
	float	dist = max(	abs(worldPos.x - tbrParam_cameraWorldPosition.x),
						abs(worldPos.y - tbrParam_cameraWorldPosition.y));
			dist = max(	dist,
						abs(worldPos.z - tbrParam_cameraWorldPosition.z) );

	return (uint)sqrt(dist / (gi_initWorldSize * 0.5f));
}

float GetVoxelizeSize(uint cascade)
{
	float cascadeScale = float(cascade + 1);
	return gi_initWorldSize * (cascadeScale * cascadeScale);
}

void ComputeVoxelizationBound(out float3 outBBMin, out float3 outBBMax, uint cascade)
{
	float cascadeScale = float(cascade + 1);

	float worldSize		= GetVoxelizeSize(cascade);
	float halfWorldSize	= worldSize * 0.5f;

	outBBMin = tbrParam_cameraWorldPosition.xyz - halfWorldSize.xxx;
	outBBMax = outBBMin + worldSize.xxx;
}

float GetDimension()
{
	return float(1 << (gi_maxCascadeWithVoxelDimensionPowOf2 & 0x0000ffff));
}

float ComputeVoxelSize(uint cascade)
{
	float worldSize = GetVoxelizeSize(cascade);
	float dimension = GetDimension();

	return worldSize / dimension;
}

float3 GetVoxelCenterPos(uint3 voxelIdx, float3 bbMin, float voxelSize)
{
	float3 voxelCenter;

	voxelCenter.x = (voxelIdx.x * voxelSize) + (voxelSize * 0.5f) + bbMin.x;
	voxelCenter.y = (voxelIdx.y * voxelSize) + (voxelSize * 0.5f) + bbMin.y;
	voxelCenter.z = (voxelIdx.z * voxelSize) + (voxelSize * 0.5f) + bbMin.z;

	return voxelCenter;
}

#endif