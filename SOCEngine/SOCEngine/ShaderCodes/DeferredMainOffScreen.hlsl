//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState linerSamplerState : register( s0 );
Texture2D<float4> offScreenBuffer : register( t0 );

float4 PS( PS_INPUT input ) : SV_Target
{
	return offScreenBuffer.Sample(linerSamplerState, input.uv);
}