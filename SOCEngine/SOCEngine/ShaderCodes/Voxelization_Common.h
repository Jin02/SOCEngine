//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLightingCommon.h"
#include "PhysicallyBased_Common.h"

#define USE_VOXELICATION_BLOATING_POS

cbuffer Voxelization_ViewProjAxis_CB : register( b5 )
{
	matrix	voxelization_vp_axisX;
	matrix	voxelization_vp_axisY;
	matrix	voxelization_vp_axisZ;
};

cbuffer Voxelization_Info_CB : register( b6 )
{
	float	voxelization_voxelizeSize;
	float	voxelization_demension;
	float3	voxelization_minPos;

	float	voxelization_voxelSize;
	uint	voxelization_currentCascade;
	float	voxelization_dummy;
};

SamplerState defaultSampler			: register( s0 );

RWTexture3D<int3> OutAnistropicVoxelTexture	: register( u0 );

//RWTexture3D<uint3> OutAnistropicVoxelTexturePosX	: register( u0 );
//RWTexture3D<uint3> OutAnistropicVoxelTextureNegX	: register( u1 );
//RWTexture3D<uint3> OutAnistropicVoxelTexturePosY	: register( u2 );
//RWTexture3D<uint3> OutAnistropicVoxelTextureNegY	: register( u3 );
//RWTexture3D<uint3> OutAnistropicVoxelTexturePosZ	: register( u4 );
//RWTexture3D<uint3> OutAnistropicVoxelTextureNegZ	: register( u5 );

#define VOXELIZATION_BLOATING_RATIO 5.0f

//if isEmission is not, input value to emission
void StoreVoxelMapAtomicColorMax(RWTexture3D<int3> voxelMap, int3 idx, float4 value, uniform bool isEmission)
{
	uint newValue = Float4ToUint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while이나 for는 그래픽 드라이버에 따라 에러가 난다고 함 -ㅠ-;
	[allow_uav_condition]do
	{
		if(isEmission)
		{
			InterlockedCompareExchange(voxelMap[idx].z, prevStoredValue, newValue, currentStoredValue);
		}
		else // albedo
		{
			InterlockedCompareExchange(voxelMap[idx].x, prevStoredValue, newValue, currentStoredValue);
		}

		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;
		
		float4 curFlt4 = UintToFloat4(currentStoredValue);
		float4 reCompute = curFlt4 + value; //걍 섞음. 뭐, 이래도 Max이긴 매한가지
		newValue = Float4ToUint(reCompute);

	}while(true);
}

void StoreVoxelMapAtomicAddNormalOneValue(RWTexture3D<int3> voxelMap, int3 idx, float value)
{
	uint newValue = asuint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while이나 for는 그래픽 드라이버에 따라 에러가 난다고 함 -ㅠ-;
	[allow_uav_condition]do
	{
		InterlockedCompareExchange(voxelMap[idx].y, prevStoredValue, newValue, currentStoredValue);
		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;
		newValue = asuint(asfloat(currentStoredValue) + value);

	}while(true);
}

#endif