#ifndef __SOC_SIMPLE_IMAGE_EFFECT_H__
#define __SOC_SIMPLE_IMAGE_EFFECT_H__

// Result -> https://www.shadertoy.com/view/XtcSR4
float4 Vignetting_Hermit(float2 uv, float2 vignetUV, float start, float end, float4 srcColor, float4 fadeColor)
{
    float dist = smoothstep(start, end, length( uv, vignetUV ) );
    return lerp(srcColor, fadeColor, saturate(fade));
}

float4 Vignetting_Linear(float2 uv, float2 vignetUV, float start, float end, float4 srcColor, float4 fadeColor)
{
    float dist = lerp(start, end, length( uv, vignetUV ) );    
    return lerp(srcColor, fadeColor, saturate(fade));
}


#endif
