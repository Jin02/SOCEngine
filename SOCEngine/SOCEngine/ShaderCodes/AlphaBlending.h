#ifndef __SOC_ALPHA_BLENDING_H__
#define __SOC_ALPHA_BLENDING_H__

float4 PremultipliedAlphaBlending(float4 front, float4 back)
{
//	float3 color = (front.a * front.rgb) + ( (1.0f - front.a) * (back.a * back.rgb) );
	float3 color = front.rgb + ((1.0f - front.a) * back.rgb);
	float alpha = front.a + (1.0f - front.a) * back.a;

	return float4(color, alpha);
}

#endif
