//USED_FOR_INCLUDE

cbuffer PCSSViewMatCB : register( b5 )
{
	matrix pcssViewMat;
}

struct PS_PCSS_VIEWDEPTH_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
	float2 viewPos					: VIEW_POS;
	float2 uv						: TEXCOORD0;
};

PS_PCSS_VIEWDEPTH_INPUT PCSSViewDepthVS(VS_INPUT input)
{
	PS_PCSS_VIEWDEPTH_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position			= mul(posWorld,						onlyPass_viewProjMat);

	float4 viewPos		= mul(posWorld, pcssViewMat);
	ps.viewPos			= float2(viewPos.z, viewPos.w);

	return ps;
}

float PCSSViewDepthPS(PS_PCSS_VIEWDEPTH_INPUT input) : SV_TARGET
{
	return input.viewPos.x / input.viewPos.y;
}

float PCSSViewDepthAlphaTestPS(PS_PCSS_VIEWDEPTH_INPUT input) : SV_TARGET
{
	float4 diffuse	= GetDiffuse(defaultSampler, input.uv);

	if(diffuse.a < ALPHA_TEST_BIAS)
		discard;

	return input.viewPos.x / input.viewPos.y;
}