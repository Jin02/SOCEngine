#ifndef __SOC_GI_COMMON_H__
#define __SOC_GI_COMMON_H__

#include "ShaderCommon.h"

cbuffer VXGIStaticInfo	: register( b6 )
{
	uint	gi_dimension;
	float	gi_maxMipLevel;
	float	gi_voxelSize;
	uint	gi_packedStepCount;
}

cbuffer VXGIDynamicInfo	: register( b7 )
{
	float3	gi_startCenterWorldPos;
	uint	gi_packedNumfOfLights;
}

uint GetDiffuseSamplingCount()
{
	return gi_packedStepCount & 0xffff;
}

uint GetSpecularSamplingCount()
{
	return gi_packedStepCount >> 16;
}

float GetVoxelizationSize()
{
	return gi_voxelSize * float(gi_dimension);
}

void ComputeVoxelizationBound(out float3 outBBMin, out float3 outBBMax, float3 startCenterWorldPos)
{
	float worldSize		= GetVoxelizationSize();
	float halfWorldSize	= worldSize * 0.5f;

	outBBMin = startCenterWorldPos - halfWorldSize.xxx;
	outBBMax = outBBMin + worldSize.xxx;
}

float3 GetVoxelCenterPos(uint3 voxelIdx, float3 bbMin, float voxelSize)
{
	float3 voxelCenter;

	voxelCenter.x = (voxelIdx.x * voxelSize) + (voxelSize * 0.5f) + bbMin.x;
	voxelCenter.y = (voxelIdx.y * voxelSize) + (voxelSize * 0.5f) + bbMin.y;
	voxelCenter.z = (voxelIdx.z * voxelSize) + (voxelSize * 0.5f) + bbMin.z;

	return voxelCenter;
}

int3 ComputeVoxelIdx(float3 minPos, float3 worldPos)
{
	return int3( (worldPos - minPos) / gi_voxelSize );
}

uint GetFlattedVoxelIndex(uint3 voxelIndex, uint dimension)
{
	uint sqDimension	= dimension * dimension;
	uint flat			= voxelIndex.x + (voxelIndex.y * dimension) + (voxelIndex.z * sqDimension);
	return flat;
}

uint GetFlattedVoxelIndexWithFaceIndex(uint3 voxelIndex, uint faceIndex, uint dimension)
{
	uint sqDimension		= dimension * dimension;

	uint fullLength			= (sqDimension * dimension);
	uint faceOffset			= fullLength * faceIndex;

	uint localFlattedIdx	= voxelIndex.x + (voxelIndex.y * dimension) + (voxelIndex.z * sqDimension);
	return faceOffset  +  localFlattedIdx;
}

#endif
