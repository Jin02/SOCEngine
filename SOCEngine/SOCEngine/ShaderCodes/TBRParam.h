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
	return float2(	tbrParam_packedViewportSize >> 16,
		tbrParam_packedViewportSize & 0x0000ffff	);
}


float4 ProjToView( float4 p )
{
	p = mul( p, tbrParam_invProjMat );
	p /= p.w;
	return p;
}

float InvertProjDepthToView(float depth)
{
	/*
	1.0f = (depth * tbrParam_invProjMat._33 + tbrParam_invProjMat._43)
	but, tbrParam_invProjMat._33 is always zero and _43 is always 1

	if you dont understand, calculate inverse projection matrix.
	but, I use inverted depth writing, so, far value is origin near value and near value is origin far value.
	*/

	return 1.0f / (depth * tbrParam_invProjMat._34 + tbrParam_invProjMat._44);
}

#endif
