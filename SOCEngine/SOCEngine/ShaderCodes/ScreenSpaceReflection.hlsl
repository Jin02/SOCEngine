struct VS_INPUT
{
	float3 position				: POSITION;
	float2 uv					: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float2 uv					: TEXCOORD0;
	float4 cameraRay			: CAMERA_RAY;
};

PS_INPUT FullScreenVS(VS_INPUT input)
{
	PS_INPUT ps = (PS_INPUT)0;

	ps.position = mul(, input.position);
	position.z	= position.w = 1.0f; //reverseeeeeeeed depth
	ps.uv		= input.uv;

	float4 cameraRayInViewSpace	= float4(ps.uv * 2.0f - 1.0f, 1.0f, 1.0f);
	cameraRayInViewSpace		= ProjToView(cameraRayInViewSpace);
	ps.cameraRayInViewSpace		= cameraRayInViewSpace;

	return ps;
}

float LinearizeDepth(float depth)
{
	return InvertProjDepthToView(depth) / tbrParam_cameraFar;
}

void swap(inout float a, inout float b)	{ float t = a; a = b; b = t; }

float DistanceSquared(float2 a, float2 b)
{
	a -= b;
	return dot(a, a);
}

float LinearDepthTexelFetch(int2 hitPixel)
{
	// Load returns 0 for any value accessed out of bounds
	return LinearizeDepth(GBufferDepth.Load(int3(hitPixel, 0)).r);
}

bool IntersectsDepthBuffer(float z, float minZ, float maxZ)
{
	/*
	* Based on how far away from the camera the depth is,
	* adding a bit of extra thickness can help improve some
	* artifacts. Driving this value up too high can cause
	* artifacts of its own.
	*/
	float depthScale = min(1.0f, z * ssrParam_strideZCutoff);
	z += ssrParam_zThickness + lerp(0.0f, 2.0f, depthScale);

	return (maxZ >= z) && (minZ - ssrParam_zThickness <= z);
}

bool RayTracing(float3 rayOrigin, float3 rayDir, float jitter,
				out float2 outHitPixel, out float3 outHitPoint)
{
	float rayLength = ((rayOrigin.z + rayDir.z * ssrParam_rayMaxDistance) < tbdrParam_cameraNear) ? 
						(tbdrParam_cameraNear - rayOrigin.z) / rayDir.z :
							ssrParam_rayMaxDistance;
	float rayEnd	= rayOrigin + rayDir * rayLength;

	float4 h0 = mul(float4(rayOrigin, 1.0f),	ssrParam_projMat);
	float4 h1 = mul(float4(rayEnd, 1.0f),		ssrParam_projMat);

	float k0 = 1.0f / h0.w;
	float k1 = 1.0f / h1.w;

	// The interpolated homogeneous version of the view space points
	float3 q0 = rayOrigin * k0;
	float3 q1 = rayEnd * k1;

	// screen space endpoints
	float2 p0 = h0.xy * k0;
	float2 p1 = h1.xy * k1;

	// if the line is degenerte, make it cover at least one pixel
	// to avoid handling zero-pixel extent as a special case later
	p1 += (DistanceSquared(p0, p1) < 0.0001f) ? float2(0.01f, 0.01f) : 0.0f;
	float2 delta = p1 - p0;

	// Permute so that the primary iteration is in x to collapse
	// all quadrant-specific DDA cases later
	bool permute = false;
	if(abs(delta.x) < abs(delta.y))
	{
		// this is a more vertical line
		permute = true;
		delta = delta.yx;
		p0 = p0.yx;
		p1 = p1.yx;
	}

	float stepDir	= sign(delta.x);
	float invdx		= stepDir / delta.x;

	// track the derivatives of q and k
	float3 dq = (q1 - q0) * invdx;
	float  dk = (k1 - k0) * invdx;
	float2 dp = float2(stepDir, delta.y * invdx);

	// Scale derivatives by the desired pixel stride and then
	// offset the starting values by the jitter fraction
	float strideScale	= 1.0f - min(1.0f, rayOrigin.z * ssrParam_strideZCutoff);
	float stride		= 1.0f + strideScale * ssrParam_stride;

	dp *= stride;		dq *= stride;		dk *= stride;
	p0 += dp * jitter;	q0 += dq * jitter;	k0 += dk * jitter;

	// Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, k from k0 to k1
	float4 pqk	= float4(p0, q0.z, k0);
	float4 dpqk	= float4(dp, dq.z, dk);
	float3 q	= q0; 

	// Adjust end condition for iteration direction
	float end = p1.x * stepDir;

	uint stepCount			= 0;
	float prevZMaxEstimate	= rayOrigin.z;
	float rayZMin			= prevZMaxEstimate;
	float rayZMax			= prevZMaxEstimate;
	float sceneZMax			= rayZMax + 100.0f;
	
	for(;	((pqk.x * stepDir) <= end) && (stepCount < ssrParam_maxStep)	&&
			(IntersectsDepthBuffer(sceneZMax, rayZMin, rayZMax) == false)	&&
			(sceneZMax != 0.0f);
			++stepCount	)
	{
		rayZMin				= prevZMaxEstimate;
		rayZMax				= (dpqk.z * 0.5f + pqk.z) / (dpqk.w * 0.5f + pqk.w);
		prevZMaxEstimate	= rayZMax;
		if(rayZMin > rayZMax)
			swap(rayZMin, rayZMax);

		outHitPixel = permute ? pqk.yx : pqk.xy;

		// You may need hitPixel.y = depthBufferSize.y - hitPixel.y; here if your vertical axis
		// is different than ours in screen space
		sceneZMax	= LinearDepthTexelFetch(int2(outHitPixel));
		pqk += dpqk;
	}

	// Advance Q based on the number of steps
	q.xy		+= dq.xy * stepCount;
	outHitPoint  = q * (1.0f / pqk.w);
	return IntersectsDepthBuffer(sceneZMax, rayZMin, rayZMax);
}

float4 ScreenSpaceReflectionPS( PS_INPUT input ) : SV_Target
{
	int3 pixelIdx = int3(input.position.xy, 0);

	float depth			= GBufferDepth.Load(pixelIdx).x;
	float3 rayOrigin	= input.cameraRayInViewSpace * LinearizeDepth(depth);

	float3 normal = GBufferNormal_roughness.Sample(GBufferDefaultSampler, input.uv);
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);
	if(any(normal == 0.0f)) discard;

	float3 rayDir = normalize( reflect(normalize(rayOrigin), normalize(normal)) );

	float2 hitPixel = float2(0.0f, 0.0f);
	float3 hitPoint = float3(0.0f, 0.0f, 0.0f);

	float jitter = (ssrParam_stride > 1.0f) ? float(int(input.position.x + input.position.y) & 1) * 0.5f : 0.0f;
	bool intersection = RayTracing(rayOrigin, rayDir, jitter, hitPixel, hitPoint);

	depth = GBufferDepth.Load(int3(hitPixel, 0)).x;
	hitPixel *= float2(texelWidth, texelHeight);

	if(	hitPixel.x > 1.0f || hitPixel.x < 0.0f ||
		hitPixel.y > 1.0f || hitPixel.y < 0.0f )
		intersection = false;

	float fade = dot(rayDir, normalize(rayOrigin));
	return float4(hitPixel, depth, fade) * (intersection ? 1.0f : 0.0f);
}