//NOT_CREATE_META_DATA

#ifndef __SOC_FULL_SCREEN_H__
#define __SOC_FULL_SCREEN_H__

#if defined(USE_VIEW_INFORMATION) | defined(USE_TBR_PARAM)
#include "TBRParam.h"
#endif

struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float2 uv					: TEXCOORD0;

#ifdef USE_VIEW_INFORMATION
	float3 viewPosition			: VIEW_POSITION;
	float3 viewRay				: VIEW_RAY;
#endif
};

PS_INPUT FullScreenVS(uint id : SV_VERTEXID)
{
	PS_INPUT ps = (PS_INPUT)0;

	float2 uv	= float2( (id << 1) & 2, id & 2);
	ps.position	= float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 1.0f, 1.0f);
	ps.uv		= uv;
	
#ifdef USE_VIEW_INFORMATION
	float4	viewSpacePosition	= mul(ps.position, tbrParam_invProjMat);
	ps.viewPosition				= viewSpacePosition.xyz;

	float4	worldSpaceViewRay	= mul(ps.position, tbrParam_invViewProjMat);
	ps.viewRay					= normalize(worldSpaceViewRay.xyz / worldSpaceViewRay.w);
#endif

	return ps;
}

#endif
