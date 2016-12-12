//EMPTY_META_DATA

#define USE_TBR_PARAM
#include "FullScreenShader.h"
#include "CommonConstBuffer.h"

cbuffer DoFParam : register(b1)
{
	float	dofParam_start;
	float	dofParam_end;
	float2	dofParam_dummy;
};

Texture2D<float4>	InputSceneMap			: register( t0 );
Texture2D<float4>	InputBlurSceneMap		: register( t1 );

SamplerState		LinearSampler			: register( s0 );


float4 DoF_InFullScreen_PS(PS_INPUT input) : SV_Target
{	
	float4 sceneMap = InputSceneMap.Sample(LinearSampler, input.uv);
	float4 blurMap	= InputBlurSceneMap.Sample(LinearSampler, input.uv);

	float depth		= InvertProjDepthToView(GBufferDepth.Sample( LinearSampler, input.uv ).x);
	
	float4 result = 0.0f;
	if(dofParam_end <= depth)			result = blurMap;
	else if(dofParam_start >= depth)	result = sceneMap;
	else
	{
		float a = (depth - dofParam_start) / (dofParam_end - dofParam_start);
		result = lerp(sceneMap, blurMap, a);
	}
	
	return result;
}
