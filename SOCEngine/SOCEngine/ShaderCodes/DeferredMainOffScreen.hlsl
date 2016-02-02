//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState linerSamplerState			: register( s0 );
Texture2D<float4> directLightColorMap	: register( t0 );

#ifdef USE_GI
Texture2D<float4> indirectLightColorMap	: register( t1 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
#ifdef USE_GI
	float4 direct	= directLightColorMap.Sample(linerSamplerState, input.uv);
	float4 indirect	= indirectLightColorMap.Sample(linerSamplerState, input.uv);

	return direct;//test
#else
	return directLightColorMap.Sample(linerSamplerState, input.uv);
#endif
}