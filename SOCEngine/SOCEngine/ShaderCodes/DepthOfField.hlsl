//EMPTY_META_DATA

#define USE_TBR_PARAM
#include "FullScreenShader.h"
#include "TBDRInput.h"
#include "CommonConstBuffer.h"

cbuffer DoFParam : register(b1)
{
	float	dofParam_focusNear;
	float	dofParam_focusFar;
	float	dofParam_blurNear;
	float	dofParam_blurFar;
};

Texture2D<float4>	InputSceneMap			: register( t0 );
Texture2D<float4>	InputBlurSceneMap		: register( t1 );

SamplerState		Sampler					: register( s0 );


float4 DoF_InFullScreen_PS(PS_INPUT input) : SV_Target
{	
	float4 sceneMap = InputSceneMap.Sample(Sampler, input.uv);
	float4 blurMap	= InputBlurSceneMap.Sample(Sampler, input.uv);

#if (MSAA_SAMPLES_COUNT > 1)
	float depth		= GBufferDepth.Load(GetViewportSize() * input.uv, 0).r;
#else
	float depth		= GBufferDepth.Sample(Sampler, input.uv).r;
#endif

	depth		= ProjDepthToView(depth);
	
	const float bn = dofParam_blurNear;
	const float bf = dofParam_blurFar;

	const float fn = dofParam_focusNear;
	const float ff = dofParam_focusFar;

#ifdef DEBUG_DOF
	float4 result = float4(1, 1, 1, 1);

	if(depth < fn)
		result.rgb = lerp(float3(1, 0, 0), float3(1, 1, 1), 1.0f - saturate((fn - depth) / (fn - bn)));
	else if(depth >= ff)
		result.rgb = lerp(float3(0, 0, 1), float3(1, 1, 1), 1.0f - saturate((depth - ff) / (bf - ff)));
	
	return result;
#else
	float a = 1.0f;

	if(depth < fn)			a = 1.0f - saturate((fn - depth) / (fn - bn));
	else if(depth >= ff)	a = 1.0f - saturate((depth - ff) / (bf - ff));

	return lerp(blurMap, sceneMap, a);
#endif
}
