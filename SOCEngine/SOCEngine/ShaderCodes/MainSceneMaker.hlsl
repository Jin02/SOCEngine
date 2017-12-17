//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState		linearSampler	: register( s0 );
Texture2D<float4>	opaqueMap		: register( t0 );
Texture2D<float4>	giMap			: register( t1 );

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
Texture2DMS<float4>	forwardPassMap	: register( t2 );
#else
Texture2D<float4>	forwardPassMap	: register( t2 );
#endif

float4 AlphaBlending(float4 front, float4 back)
{
	return front + back * (1.0f - front.a);
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 opaque	= opaqueMap.Sample(linearSampler, input.uv);
	float4 gi		= giMap.Sample(linearSampler, input.uv);

	opaque = min(opaque + gi, 1.0f);

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 forwardPassColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll] for (uint i = 0; i < 4; ++i)
		forwardPassColor += forwardPassMap.Load(input.position.xy, i);

	forwardPassColor /= 4.0f;
#else
	float4 forwardPassColor = forwardPassMap.Sample(linearSampler, input.uv);
#endif

	return AlphaBlending(forwardPassColor, opaque);
}