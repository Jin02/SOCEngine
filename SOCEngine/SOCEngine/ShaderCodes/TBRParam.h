//NOT_CREATE_META_DATA

#ifndef __SOC_TBR_PARAM_H__
#define __SOC_TBR_PARAM_H__

cbuffer TBRParam : register( b0 )
{
	matrix 	tbrParam_invProjMat;
	matrix 	tbrParam_invViewProjMat;
	matrix	tbrParam_invViewProjViewportMat;

	uint	tbrParam_packedViewportSize;
	uint 	tbrParam_packedNumOfLights;
	uint	tbrParam_maxNumOfPerLightInTile;
	float	tbrParam_gamma;
};

float GetGamma()
{
	return tbrParam_gamma;
}

float2 GetViewportSize()
{
	return float2(tbrParam_packedViewportSize >> 16, tbrParam_packedViewportSize & 0x0000ffff);
}


float4 ProjToView( float4 p )
{
	p = mul( p, tbrParam_invProjMat );
	p /= p.w;
	return p;
}

float ProjDepthToView(float depth)
{
	// 1.0f = ( (x * _14) + (y * _24) + (z * _34) + _44 )
	// 1.0f = ( z * _34 + _44 )
	// ViewSpace Depth = 1.0f / (z * _34 + _44)
	return 1.0f / (depth * tbrParam_invProjMat._34 + tbrParam_invProjMat._44);
}


struct Surface
{
	float3	worldPos;
	float	depth;

	float3	albedo;
	float	occlusion;

	float3	normal;
	float	roughness;

	float3	specular;
	float	metallic;

	float3	emission;
	float	specularity;

	float2	motion;
	uint	materialFlag;
};


#endif
