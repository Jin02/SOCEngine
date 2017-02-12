//EMPTY_META_DATA

#define USE_TBR_PARAM
#include "FullScreenShader.h"
#include "CommonConstBuffer.h"

cbuffer DoFParam : register(b1)
{
	float	dofParam_focusNear;
	float	dofParam_blurNear;
	float	dofParam_focusFar;
	float	dofParam_blurFar;
};

Texture2D<float4>	InputSceneMap			: register( t0 );
Texture2D<float4>	InputBlurSceneMap		: register( t1 );

SamplerState		LinearSampler			: register( s0 );


float4 DoF_InFullScreen_PS(PS_INPUT input) : SV_Target
{	
	float4 sceneMap = InputSceneMap.Sample(LinearSampler, input.uv);
	float4 blurMap	= InputBlurSceneMap.Sample(LinearSampler, input.uv);

	float depth		= InvertProjDepthToView(GBufferDepth.Sample( LinearSampler, input.uv ).x);
	
#if 1
	float a		= 0.0f;
	if(depth < dofParam_focusNear)		a = min(dofParam_blurNear / depth, 1.0f);		
	else if(depth > dofParam_focusFar)	a = min((depth - dofParam_focusFar) / (dofParam_blurFar - dofParam_focusFar), 1.0f);
		
	return lerp(sceneMap, blurMap, a);
#else
	float a = (depth - dofParam_focusNear) / (dofParam_focusFar - dofParam_focusNear);	
	return lerp(blurMap, sceneMap, a);
#endif
}
