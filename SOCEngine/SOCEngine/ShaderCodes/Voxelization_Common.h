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
	float	voxelization_voxelSize;
	float	voxelization_demension;

	float3	voxelization_camPosition;
	float2	voxelization_dummy;
};

SamplerState defaultSampler 	: register( s0 );

RWTexture3D<float4> OutVoxelTexture : register(u0);

#define VOXELIZATION_BLOATING_RATIO 5.0f
#define VOXELIZATION_DEBUG_MODE

#endif