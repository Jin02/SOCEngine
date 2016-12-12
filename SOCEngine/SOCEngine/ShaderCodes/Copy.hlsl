//EMPTY_META_DATA

#include "FullScreenShader.h"

Texture2D<float4>	InputColorMap : register( t0 );
SamplerState		LinearSampler : register( s0 );

float4 Copy_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	return InputColorMap.SampleLevel(LinearSampler, input.uv, 0);
}