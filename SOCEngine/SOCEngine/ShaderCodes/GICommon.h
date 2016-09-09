#ifndef __SOC_GI_COMMON_H__
#define __SOC_GI_COMMON_H__

#include "ShaderCommon.h"
#include "VoxelRawBufferCommon.h"

#ifndef VOXEL_CONE_TRACING
cbuffer GIGlobalStaticInfo : register( b6 )
#else
cbuffer GIGlobalStaticInfo : register( b1 )
#endif
{
	// High 16 bit is cascade
	// Low bits is voxel dimension
	uint	gi_maxCascadeWithVoxelDimensionPowOf2;
	float	gi_maxMipLevel;

	float	gi_occlusion;
	float	gi_diffuseHalfConeMaxAngle;
}

#ifndef VOXEL_CONE_TRACING
cbuffer GIGlobalDynamicInfo : register( b7 )
#else
cbuffer GIGlobalDynamicInfo : register( b2 )
#endif
{
	float	gi_initVoxelSize;
	float3	gi_startCenterWorldPos;
}

uint GetMaximumCascade()
{
	return (gi_maxCascadeWithVoxelDimensionPowOf2 >> 16);
}

float GetDimension()
{
	return float(1 << (gi_maxCascadeWithVoxelDimensionPowOf2 & 0x0000ffff));
}

float GetInitWorldSize()
{
	return gi_initVoxelSize * GetDimension();
}

uint ComputeCascade(float3 worldPos, float3 cameraWorldPos)
{
	//x, y, z 축 중에 가장 큰걸 고름
	float	dist = max(	abs(worldPos.x - cameraWorldPos.x),
						abs(worldPos.y - cameraWorldPos.y));
			dist = max(	dist,
						abs(worldPos.z - cameraWorldPos.z) );

	return (uint)sqrt(dist / (GetInitWorldSize() * 0.5f));
}

float GetVoxelizeSize(uint cascade)
{
	float cascadeScale = float(cascade + 1);
	return GetInitWorldSize() * (cascadeScale * cascadeScale);
}

void ComputeVoxelizationBound(out float3 outBBMin, out float3 outBBMax, uint cascade, float3 cameraWorldPos)
{
	float cascadeScale = float(cascade + 1);

	float worldSize		= GetVoxelizeSize(cascade);
	float halfWorldSize	= worldSize * 0.5f;

	outBBMin = cameraWorldPos - halfWorldSize.xxx;
	outBBMax = outBBMin + worldSize.xxx;
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

void StoreVoxelMapAtomicColorAvgUsingRawBuffer(RWByteAddressBuffer voxelMap, uint flattedVoxelIdx, float4 value, uniform bool useLimit)
{
	value *= 255.0f;

	uint newValue			= ToUint(value);
	uint prevStoredValue	= 0;
	uint currentStoredValue	= 0;

	uint count = 0;

	// 현재 개발환경에서 while과 for는 작동이 되질 않는다.
	// 왜 그런지는 모르겠지만, 유일하게 do-while만 작동이 되는 상태.
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

void StoreVoxelMapAtomicColorAvgUsingTexture3D(RWTexture3D<uint> voxelMap, int3 idx, float4 value, uniform bool useLimit)
{
	value *= 255.0f;

	uint newValue			= ToUint(value);
	uint prevStoredValue	= 0;
	uint currentStoredValue	= 0;

	uint count = 0;

	// 현재 개발환경에서 while과 for는 작동이 되질 않는다.
	// 왜 그런지는 모르겠지만, 유일하게 do-while만 작동이 되는 상태.
	[allow_uav_condition]do//[allow_uav_condition]while(true)
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);

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


void StoreRadiosityUsingRawBuffer(RWByteAddressBuffer outVoxelColorMap, float3 radiosity, float alpha, float3 normal, uint3 voxelIdx, uint curCascade)
{
	float anisotropicNormals[6] = {
		-normal.x,
		 normal.x,
		-normal.y,
		 normal.y,
		-normal.z,
		 normal.z
	};

	for(int faceIndex=0; faceIndex<6; ++faceIndex)
	{
		float rate = max(anisotropicNormals[faceIndex], 0.0f);
		float4 storeValue = float4(radiosity * rate, alpha);

		uint flattedIndex = GetFlattedVoxelIndexWithFaceIndex(voxelIdx, curCascade, faceIndex, uint(GetDimension()));
		StoreVoxelMapAtomicColorAvgUsingRawBuffer(outVoxelColorMap, flattedIndex, storeValue, true);
	}
}

void StoreRadiosityUsingRWTexture3D(RWTexture3D<uint> outVoxelColorMap, float3 radiosity, float alpha, float3 normal, uint3 voxelIdx, uint curCascade)
{
	float anisotropicNormals[6] = {
		-normal.x,
		 normal.x,
		-normal.y,
		 normal.y,
		-normal.z,
		 normal.z
	};

	uint dimension = (uint)GetDimension();
	voxelIdx.y += curCascade * dimension;

	for(int faceIndex=0; faceIndex<6; ++faceIndex)
	{
		uint3 index = voxelIdx;
		index.x += (faceIndex * dimension);

		float rate = max(anisotropicNormals[faceIndex], 0.0f);
		float4 storeValue = float4(radiosity * rate, alpha);

		outVoxelColorMap[index] = Float4ColorToUint(storeValue);
	}
}



#endif
