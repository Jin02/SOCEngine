//EMPTY_META_DATA
// #16_0 기반으로 작업 중..

cbuffer ScreenSpaceRayTracing_ViewToTexSpace	: register(b?)
{
	matrix	ssrt_viewToTextureSpace;
}

cbuffer ScreenSpaceRayTracing			: register(b?)
{
	float	ssrt_thickness;
	float	ssrt_maxDistance;
	float	ssrt_strideZCutoff;
	float	ssrt_stride;

	float	ssrt_maxStepCount;
	float	ssrt_maxMipLevel;	// use for SSLR
	float	ssrt_fadeStart;		// use for SSLR
	float	ssrt_fadeEnd;		// use for SSLR
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

float FetchLinerDepthFromGBuffer(int2 screenPos)
{
	float depth = GBufferDepth.Load(int3(screenPos, 0).r;
	return LinearizeDepth(depth);
}

bool TraceScreenSpaceRay(out float2 outHitScreenPos, out float3 outHitPos,
							float3 rayOrigin, float3 rayDir, float jitter	)
{
	float rayLength		= ((rayOrigin.z + rayDir.z * ssrt_maxDistance) < tbrParam_cameraNear) ?
			   	   (tbrParam_cameraNear - rayOrigin.z) / rayDir.z : ssrt_maxDistance;
	float3 rayEndPos	= rayOrigin + rayDir * rayLength;
	
	float2 viewportSize	= GetViewportSize();


	// homogeneous 공간으로 투영
	float4 h0	= mul( float4(rayOrigin, 1.0f), ssrt_viewToTextureSpace );
	h0.xy		*= viewportSize;
	float k0	= 1.0f / h0.w;
	
	float4 h1	= mul( float4(rayEndPos, 1.0f), ssrt_viewToTextureSpace );
	h1.xy		*= viewportSize;
	float k1	= 1.0f / h1.w;
	
	float3 q0	= rayOrigin * k0;
	float3 q1	= rayEndPos * k1;
	
	float2 p0	= h0.xy * k0;
	float2 p1	= h1.xy * k1;
	// line이 생성되지 않았다면, 픽셀하나 정도만 되도록 수정
	p1		+= (Distance_2(p0, p1) < 0.001f) ? float2(0.01f, 0.01f) : 0.0f;


	float2 delta = p1 - p0;
	bool permute = false;
	if(abs(delta.x) < abs(delta.y))
	{
		delta	= delta.yx;
		p0	= p0.yx;
		p1	= p1.yx;
		
		permute	= true;
	}

	float stepDir		= sign(delta.x);
	float invdx		= stepDir / delta.x;
	
	float3 dq		= (q1 - q0) * invdx;
	float  dk		= (k1 - k0) * invdx;
	float2 dp		= float2(stepDir, delta.y * invdx);
	
	float strideScale	= 1.0f - min(1.0f, rayOrigin.z * ssrt_strideZCutoff);
	float stride		= 1.0f + strideScale * ssrt_stride;
	
	dq		*= stride;
	dk		*= stride;
	dp		*= stride;
	
	p0		+= dp * jitter;
	q0		+= dq * jitter;
	k0		+= dk * jitter;
	
	float4 pqk	= float4(p0, q0.z, k0);
	float4 dpqk	= float4(dp, dq.z, dk);
	float3 q	= q0;
	
	float end	= p1.x * stepDir;
	float prevMaxZ	= rayOrigin.z;
	float rayMinZ	= prevMaxZ;
	float rayMaxZ	= prevMaxZ;
	float sceneMaxZ	= rayMaxZ + 100.0f;
	float stepCount	= 0.0f;
	
	for(;

		((pqk.x * stepDir) <= end) &&
		(stepCount < ssrt_maxStepCount) &&
		(IntersectDepth(sceneMaxZ, rayMinZ, rayMaxZ) == false) &&
		(sceneMaxZ != 0.0f);

		stepCount += 1.0f)
		{
			rayMinZ 	= prevMaxZ;
			rayMaxZ		= (dpqk.z * 0.5f + pqk.z) / (dpqk.w * 0.5f, + pqk.w);
			prevMaxZ	= rayMaxZ;
			
			if(rayMinZ > rayMaxZ)
				swap(rayMinZ, rayMaxZ);
			
			outHitScreenPos	= permute ? pqk.yx : pqk.xy;
			sceneMaxZ	= FetchLinerDepthFromGBuffer( int2(outHitScreenPos) );
			
			pqk += dpqk;
		}

	q.xy		+= dq.xy * float(stepCount);
	outHitPos	= q * (1.0f / pqk.w);
	
	return IntersectDepth(sceneMaxZ, rayMinZ, rayMaxZ);
}
