#ifndef __SOC_COMMON_CONST_BUFFER_H__
#define __SOC_COMMON_CONST_BUFFER_H__

cbuffer Transform : register( b1 )		//Object World
{
	matrix transform_world;
	matrix transform_worldInvTranspose;

	matrix transform_prevWorld;		// used for motion blur
};

cbuffer Camera : register( b2 )
{
	matrix	camera_viewMat;			// or InvNearFarViewProj
	matrix	camera_viewProjMat;
	matrix	camera_prevViewProjMat;		// used for motion blur

	float3	camera_worldPos;
	uint	camera_packedNearFar;
};

float GetCameraNear()
{
	return f16tof32(camera_packedNearFar >> 16);
}

float GetCameraFar()
{
	return f16tof32(camera_packedNearFar & 0x0000ffff);
}

#endif
