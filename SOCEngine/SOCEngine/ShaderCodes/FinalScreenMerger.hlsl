//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "ShaderCommon.h"

SamplerState		renderSceneSamplerState	: register( s0 );
Texture2D<float4>	renderScene		: register( t0 );

#if defined(USE_UI)
SamplerState uiSceneSamplerState		: register( s1 );
Texture2D<float4> uiScene			: register( t1 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 renderSceneColor	= renderScene.Sample(renderSceneSamplerState, input.uv);

#if defined(USE_UI)
	float4 uiSceneColor	= uiScene.Sample(uiSceneSamplerState, input.uv);
	finalColor		= renderSceneColor + uiSceneColor;
#else
	finalColor		= renderSceneColor;
#endif

	return finalColor;
}
