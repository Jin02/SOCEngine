//USED_FOR_INCLUDE

struct PS_MOMENT_DEPTH_INPUT
{
	float4	position 	 	: SV_POSITION;
	float	depth			: DEPTH;
#if defined(ENABLE_ALPHA_TEST)
	float2	uv			: TEXCOORD0;
#endif
};

PS_MOMENT_DEPTH_INPUT MomentDepthVS(VS_INPUT input)
{
	PS_MOMENT_DEPTH_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position		= mul(posWorld, camera_viewProjMat);

#if defined(USE_SHADOW_INVERTED_DEPTH)
	float4 invPos		= mul(posWorld, camera_viewMat); // ShadowMap에서 사용하는 viewMat은 invertedViewProjMat임.
	ps.depth		= invPos.z / invPos.w;
#else
	ps.depth		= ps.position.z / ps.position.w;
#endif

#if defined(ENABLE_ALPHA_TEST)
	ps.uv			= input.uv;
#endif

	return ps;
}

float4 DistributePrecision(float2 moment)
{  
	float distFactor	= 256.0f;
	float invFactor		= 1.0f / distFactor;  

	// Split precision  
	float2 intPart;  
	float2 fracPart		= modf(moment * distFactor, intPart); // 정수, 소수점부분을 분리

	// Compose outputs to make reconstruction cheap.  
	return float4(intPart * invFactor, fracPart);  
}  

float4 MomentDepthPS(PS_MOMENT_DEPTH_INPUT input) : SV_TARGET
{
	float depth = input.depth;

	float2 moment = float2(depth, depth * depth);

	float dx = ddx(depth);
	float dy = ddy(depth);

	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	moment.y += 0.25f * (dx * dx + dy * dy);
	float4 outColor = DistributePrecision(moment);

#if defined(ENABLE_ALPHA_TEST)
	float alpha = GetAlpha(defaultSampler, input.uv);
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	return outColor;
}
