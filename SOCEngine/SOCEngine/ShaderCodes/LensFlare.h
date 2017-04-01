#ifndef __SOC_LENS_FLARE_H__
#define __SOC_LENS_FLARE_H__

float3 LensFlare_1(float2 uv, float2 pos)
{
	float2 main	= uv - pos;
	float2 uvd	= uv * (length(uv));
	
	float	ang	= atan(main.x, main.y);
	float	dist	= length(main);
		dist	= pow(dist, 0.1f);
		
	float f1	= max(0.01f - pow(length(uv+ 1.2f * pos), 1.9f), 0.0f) * 7.0f;
	float f2	= max(1.0f / (1.0f + 32.0f * pow(length(uvd + 0.8f  * pos), 2.0f)), 0.0f) * 0.25f;
	float f22	= max(1.0f / (1.0f + 32.0f * pow(length(uvd + 0.85f * pos), 2.0f)), 0.0f) * 0.23f;
	float f23	= max(1.0f / (1.0f + 32.0f * pow(length(uvd + 0.9f  * pos), 2.0f)), 0.0f) * 0.21f;
	
	vec2 uvx = lerp(uv, uvd, -0.5f);
	
	float f4	= max(0.01f - pow(length(uvx + 0.4f  * pos),	2.4f), 0.0f) * 6.0f;
	float f42	= max(0.01f - pow(length(uvx + 0.45f * pos),	2.4f), 0.0f) * 5.0f;
	float f43	= max(0.01f - pow(length(uvx + 0.5f  * pos),	2.4f), 0.0f) * 3.0f;
	
	uvx = lerp(uv, uvd, -0.4f);
	
	float f5	= max(0.01f - pow(length(uvx + 0.2f * pos), 5.5f), 0.0f) * 2.0f;
	float f52	= max(0.01f - pow(length(uvx + 0.4f * pos), 5.5f), 0.0f) * 2.0f;
	float f53	= max(0.01f - pow(length(uvx + 0.6f * pos), 5.5f), 0.0f) * 2.0f;
	
	uvx = lerp(uv, uvd, -0.5f);
	
	float f6	= max(0.01f - pow(length(uvx - 0.3  f * pos), 1.6f), 0.0f) * 6.0f;
	float f62	= max(0.01f - pow(length(uvx - 0.325f * pos), 1.6f), 0.0f) * 3.0f;
	float f63	= max(0.01f - pow(length(uvx - 0.35 f * pos), 1.6f), 0.0f) * 5.0f;
	
	float3 c = float3(f2  + f4  + f5  + f6,
			  f22 + f42 + f52 + f62,
			  f23 + f43 + f53 + f63) * 1.3f - float3( (length(uvd) * 0.05f).xxx );
	
	return c;
}

float2 GetDistOffset(float2 uv, float2 pxoffset, uniform float DistortionBarrel)
{
	float2 tocenter		= uv.xy;
	float3 prep		= normalize(float3(tocenter.y, -tocenter.x, 0.0f));
	
	float angle		= length(tocenter.xy) * 2.221f * DistortionBarrel;
	float3 oldoffset 	= float3(pxoffset, 0.0f);
	
	float3 rotated = oldoffset * cos(angle) + cross(prep, oldoffset) * sin(angle) + prep * dot(prep, oldoffset) * (1.0f - cos(angle));    
	return rotated.xy;
}

float3 Flare(float2 uv, float2 pos, float dist, float size, uniform float DistortionBarrel)
{
	pos = GetDistOffset(uv, pos, DistortionBarrel);
    
	float r = max(0.01f - pow(length(uv + (dist - 0.05f) * pos), 2.4f) * (1.0f / (size * 2.0f)), 0.0f) * 6.0f;
	float g = max(0.01f - pow(length(uv +  dist          * pos), 2.4f) * (1.0f / (size * 2.0f)), 0.0f) * 6.0f;
	float b = max(0.01f - pow(length(uv + (dist + 0.05f) * pos), 2.4f) * (1.0f / (size * 2.0f)), 0.0f) * 6.0f;
    
	return float3(r, g, b);
}

float3 Flare(float2 uv, float2 pos, float dist, float size, float3 color, uniform float DistortionBarrel)
{
    return Flare(uv, pos, dist, size, DistortionBarrel) * color;
}

float3 Orb(float2 uv, float2 pos, float dist, float size, uniform uint OrbFlareCount, uniform float DistortionBarrel)
{
	float3 c = float3(0.0f, 0.0f, 0.0f);
	
	for(uint i=0; i<OrbFlareCount; i++)
	{
		float j		= float(i + 1);
		float offset	= j / (j + 1.0f);
		float colOffset	= j / float(OrbFlareCount * 2);
		
		c += Flare(uv, pos, dist + offset, size / (j + 0.1f), float3(1.0f - colOffset, 1.0f, 0.5f + colOffset), DistortionBarrel);
	}
	
	c += Flare(uv, pos, dist + 0.5f, 4.0f * size, float3(1.0f, 1.0f, 1.0f), DistortionBarrel) * 4.0f; 
	return c / 4.0f;
}

float3 Orb(float2 uv, float2 pos, float dist, float size, float3 color, uniform uint OrbFlareCount, uniform float DistortionBarrel)
{
	return Orb(uv, pos, dist, size, OrbFlareCount, DistortionBarrel) * color;
}

float3 Ring(float2 uv, float2 pos, float dist)
{
	float2 uvd = uv * (length(uv));
    
	float r = max(1.0f / (1.0f + 32.0f * pow(length(uvd+(dist - 0.05f) * pos), 2.0f)), 0.0f) * 0.25f;
	float g = max(1.0f / (1.0f + 32.0f * pow(length(uvd+ dist          * pos), 2.0f)), 0.0f) * 0.23f;
	float b = max(1.0f / (1.0f + 32.0f * pow(length(uvd+(dist + 0.05f) * pos), 2.0f)), 0.0f) * 0.21f;
    
	return float3(r, g, b);
}

float3 Lensflare2(float2 uv, float2 pos, float brightness, float size, uniform uint OrbFlareCount, uniform float DistortionBarrel)
{
    float3 c = float3(0.1f, 0.1f, 0.1f);

    c += Flare(uv, pos, -3.0f, 3.0f * size, DistortionBarrel);
    c += Flare(uv, pos, -1.0f, size, DistortionBarrel) * 3.0f;
    c += Flare(uv, pos,  0.5f, 0.8f * size, DistortionBarrel);
    c += Flare(uv, pos, -0.4f, 0.8f * size, DistortionBarrel);
    
    c += Orb(uv,pos, 0.0f, 0.5f * size, OrbFlareCount, DistortionBarrel);
    
    c += Ring(uv, pos, -1.0f) * 0.5f * size;
    c += Ring(uv, pos,  1.0f) * 0.5f * size;
    
    return c * brightness;
}


#endif
