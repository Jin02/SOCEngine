//EMPTY_META_DATA

#define USE_TBR_PARAM
#include "FullScreenShader.h"

cbuffer DoFParam : register(b1)
{
	float	dofParam_foucsNear;
	float	dofParam_blurNear;
	float	dofParam_foucsFar;
	float	dofParam_blurFar;
};

Texture2D<float4>	InputSceneMap			: register( t0 );
Texture2D<float4>	InputBlurSceneMap		: register( t1 );

SamplerState		LinearSampler			: register( s0 );

float3 ComputeWorldPos(float2 uv)
{
	uint2 screenPos = uint2(uv * GetViewportSize());
	
#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	float depth = GBufferDepth.Load( screenPos, sampleIdx ).x;
#else
	float depth = GBufferDepth.Load( uint3(screenPos, 0) ).x;
#endif

	float4	worldPos	= mul(float4(screenPos, depth, 1.0f), tbrParam_invViewProjViewportMat);
	return (worldPos.xyz / worldPos.w);
}

float4 DoF_InFullScreen_PS(PS_INPUT input) : SV_Target
{	
	float3 worldPos = ComputeWorldPos(input.uv);
	
	float dist	= length(worldPos, tbrParam_cameraWorldPosition);
	float a		= 0.0f;

	if(dist < dofParam_focusNear)		a = min(dofParam_blurNear / dist, 1.0f);		
	else if(dist > dofParam_focusFar)	a = min((dist - dofParam_focusFar) / (dofParam_blurFar - dofParam_foucsFar), 1.0f);
	
	float4 sceneMap = InputSceneMap.Sample(LinearSampler, input.uv);
	float4 blurMap	= InputBlurSceneMap.Sample(LinearSampler, input.uv);
	
	return lerp(sceneMap, blurMap, a);
}
