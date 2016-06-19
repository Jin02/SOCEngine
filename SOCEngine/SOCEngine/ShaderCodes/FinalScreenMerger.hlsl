//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "ShaderCommon.h"

SamplerState		RenderSceneSamplerState	: register( s0 );
Texture2D<float4>	RenderScene		: register( t0 );

#if defined(USE_UI)
SamplerState UISceneSamplerState		: register( s1 );
Texture2D<float4> UIScene			: register( t1 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 RenderSceneColor	= RenderScene.Sample(RenderSceneSamplerState, input.uv);

#if defined(USE_UI)
	float4 UISceneColor	= UIScene.Sample(UISceneSamplerState, input.uv);
	finalColor		= RenderSceneColor + UISceneColor;
#else
	finalColor		= RenderSceneColor;
#endif

	return finalColor;
}
