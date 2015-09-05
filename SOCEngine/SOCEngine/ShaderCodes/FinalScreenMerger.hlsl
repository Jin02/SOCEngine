//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState renderSceneSamplerState	: register( s0 );

#if (MSAA_SAMPLES_COUNT > 1)
Texture2DMS<float4, MSAA_SAMPLES_COUNT>	renderScene		: register( t0 );
#else
Texture2D<float4>						renderScene		: register( t0 );
#endif

#if defined(USE_UI)
SamplerState uiSceneSamplerState		: register( s1 );
Texture2D<float4> uiScene				: register( t1 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
#if (MSAA_SAMPLES_COUNT > 1)
	int2 texCoord = int2((int)(input.uv.x * 0), (int)(input.uv.y * 0));
	float4 renderSceneColor = renderScene.Load(texCoord.xy, input.sampleIdx);
#else
	float4 renderSceneColor = renderScene.Sample(renderSceneSamplerState, input.uv);
#endif

#if defined(USE_UI)
	float4 uiSceneColor = uiScene.Sample(uiSceneSamplerState, input.uv);
	finalColor = renderSceneColor + uiSceneColor;
#else
	finalColor = renderSceneColor;
#endif

	return finalColor;
}