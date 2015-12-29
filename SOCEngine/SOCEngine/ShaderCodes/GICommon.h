#ifndef __SOC_GI_COMMON_H__
#define __SOC_GI_COMMON_H__

#include "ShaderCommon.h"

cbuffer GIInfoCB : register( b6 )
{
	uint	gi_maxCascade;
	uint	gi_voxelDimension;
	float	gi_initVoxelSize;

	float3	gi_initWorldMinPos;
	float	gi_initWorldSize;
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

float ComputeVoxelSize(uint cascade)
{
	float worldSize = GetVoxelizeSize(cascade);
	return worldSize / (float)gi_voxelDimension;
}

#endif