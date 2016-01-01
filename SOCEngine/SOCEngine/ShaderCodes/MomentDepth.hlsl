//USED_FOR_INCLUDE

struct PS_MOMENT_DEPTH_INPUT
{
	float4	position 	 	: SV_POSITION;
	float	depth			: DEPTH;
};

PS_MOMENT_DEPTH_INPUT MomentDepthVS(VS_INPUT input)
{
	PS_MOMENT_DEPTH_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position			= mul(posWorld, cameraMat_viewProj);
	ps.depth			= ps.position.z / ps.position.w;

	return ps;
}

float MomentDepthPS(PS_MOMENT_DEPTH_INPUT input) : SV_TARGET
{
	float depth = input.depth;

	float2 moment = float2(depth, depth * depth);

	float dx = ddx(depth);
	float dy = ddy(depth);

	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	moment.y += 0.25f * (dx * dx + dy * dy);

#if defined(ENABLE_ALPHA_TEST)
	float4 diffuseTex = diffuseTexture.Sample(defaultSampler, input.uv);
	float opacityMap = 1.0f - opacityTexture.Sample(defaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap * ParseMaterialAlpha();
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	// x는 이미 다른곳에서 기록 중
	return moment.y;
}