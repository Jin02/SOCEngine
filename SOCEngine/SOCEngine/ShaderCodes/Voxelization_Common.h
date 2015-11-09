//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLightingCommon.h"

cbuffer Voxelization_ViewProjAxis_CB : register( b5 )
{
	matrix	voxelization_vp_axisX;
	matrix	voxelization_vp_axisY;
	matrix	voxelization_vp_axisZ;
};

cbuffer Voxelization_Info_CB : register( b6 )
{
	float3	voxelization_minVertexWorldPos;
	float	voxelization_elementSize;
};

RWTexture3D OutVoxelTexture : register(u0);

#define VOXELIZATION_BLOATING_RATIO 5.0f
#define VOXELIZATION_DEBUG_MODE

#endif