//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState		linearSampler	: register( s0 );
Texture2D<float4>	opaqueMap		: register( t0 );
Texture2D<float4>	giMap			: register( t1 );

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
Texture2DMS<float4>	transparentMap	: register( t2 );
#else
Texture2D<float4>	transparentMap	: register( t2 );
#endif

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
Texture2DMS<float4>	skyBoxMap		: register( t3 );
#else
Texture2D<float4>	skyBoxMap		: register( t3 );
#endif

float4 AlphaBlending(float4 front, float4 back)
{
	return front + back * (1.0f - front.a);
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 opaque	= opaqueMap.Sample(linearSampler, input.uv);
	float4 gi		= giMap.Sample(linearSampler, input.uv);

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 transparentColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll] for (uint i = 0; i < 4; ++i)
		transparentColor += transparentMap.Load(input.position.xy, i);
	transparentColor /= 4.0f;

	float4 skyBoxColor		= skyBoxMap.Load(input.position.xy, 0);
#else
	float4 transparentColor = transparentMap.Sample(linearSampler, input.uv);
	float4 skyBoxColor = skyBoxMap.Sample(linearSampler, input.uv);
#endif

	opaque = min(opaque + gi + skyBoxColor, 1.0f);
	return AlphaBlending(transparentColor, opaque);
}