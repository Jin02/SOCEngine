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

void Swap(inout float a, inout float b)	{	float t = a;	a = b;    b = t;	}

float Distance_2(float2 a, float2 b)
{
    float2 x = a - b;
    return dot(x, x);
}


bool IntersectDepth(float z, float minZ, float maxZ)
{
	// thickness를 통해 약간의 artifact를 수정 가능
	// 그렇다고 너무 올리면 더 잘 안돌아가고..
    float scale = min(1.0f, z * ssrt_strideZCutoff);
    z += ssrt_thickness + lerp(0.0f, 2.0f, scale);
    
    return (maxZ >= z) && (minZ - ssrt_thickness <= z);
}

// LightCullingCommonCS.h에 있던 함수 그대로 긁어옴.
// 라이트 컬링 파일을 수정해서, 좀 여기서도 자유롭게 쓸 수 있도록 수정해야함.
// TBDRInput에 넣든가, 뭐 그건 알아서 하면 된다고 생각

// 특히, 이 프로젝트는 다른 방식과 같은 LinearDepth를 작성하지 않기에, 이 방식을 꼭 사용해야 함
float InvertProjDepthToView(float depth)
{
	/*
	1.0f = (depth * tbrParam_invProjMat._33 + tbrParam_invProjMat._43)
	but, tbrParam_invProjMat._33 is always zero and _43 is always 1
		
	if you dont understand, calculate inverse projection matrix.
	but, I use inverted depth writing, so, far value is origin near value and near value is origin far value.
	*/

	return 1.0f / (depth * tbrParam_invProjMat._34 + tbrParam_invProjMat._44);
}

float LinearizeDepth(float depth)
{
	return InvertProjDepthToView(depth) / tbrParam_cameraFar;
}

