//NOT_CREATE_META_DATA

#ifndef __SOC_FULL_SCREEN_H__
#define __SOC_FULL_SCREEN_H__

#ifdef USE_VIEW_INFORMATION
#include "TBDRInput.h"
#endif

struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float2 uv					: TEXCOORD0;

#ifdef USE_VIEW_INFORMATION
	float3 viewPosition			: VIEW_POSITION;
	float3 viewRay				: VIEW_RAY;
#endif

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	uint sampleIdx				: SV_SAMPLEINDEX;
#endif
};

PS_INPUT FullScreenVS(uint id : SV_VERTEXID)
{
	PS_INPUT ps = (PS_INPUT)0;

	ps.position.x = (float)(id / 2) * 4.0f - 1.0f;
	ps.position.y = (float)(id % 2) * 4.0f - 1.0f;
	ps.position.z = 1.0f; //inverted depth
	ps.position.w = 1.0f;

	ps.uv.x = (float)(id / 2) * 2.0f;
	ps.uv.y = 1.0f - (float)(id % 2) * 2.0f;
	
	
#ifdef USE_VIEW_INFORMATION
	float4	viewSpacePosition	= mul(ps.position, tbrParam_invProjMat);
	ps.viewPosition				= viewSpacePosition.xyz;

	float4	worldSpaceViewRay	= mul(ps.position, tbrParam_invViewProjMat);
	ps.viewRay					= normalize(worldSpaceViewRay.xyz / worldSpaceViewRay.w);
#endif

	return ps;
}

#endif