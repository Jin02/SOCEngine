//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState samplerState				: register( s0 );

#if (MSAA_SAMPLES_COUNT > 1)
Texture2DMS<float4,MSAA_SAMPLES_COUNT>	offScreenBuffer	: register( t0 );
#else
Texture2D<float4>						offScreenBuffer	: register( t0 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
#if (MSAA_SAMPLES_COUNT > 1)
	int2 pos = int2(input.position.xy);
	return offScreenBuffer.Load(pos.xy, input.sampleIdx);
#else
	return offScreenBuffer.SampleLevel(samplerState, input.uv, 0);
#endif
}