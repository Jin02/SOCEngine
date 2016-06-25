//EMPTY_META_DATA
// #16_0 기반으로 작업 중..

cbuffer ScreenSpaceRayTracing : register(b?)
{
	float	ssrt_thickness;
	float	ssrt_maxStep;
	float	ssrt_maxDistance;
	float	ssrt_strideZCutoff;

	float	ssrt_maxMipLevel;
	float	ssrt_fadeStart;
	float	ssrt_fadeEnd;
	
//	float	ssrt_dummy;
	float	ssrt_padding;
};

void Swap(inout float a, inout float b)
{
    float t = a;
    a = b;
    b = t;
}

float Distance_2(float2 a, float2 b)
{
    float2 x = a - b;
    return dot(x, x);
}


bool IntersectDepth(float z, float minZ, float maxZ)
{
    //
    float scale = min(1.0f, z * ssrt_strideZCutoff);
    z += ssrt_thickness + lerp(0.0f, 2.0f, scale);
    
    return (maxZ >= z) && (minZ - ssrt_thickness <= z);
}
