//USED_FOR_INCLUDE

struct PS_ALPHA_TEST_DIFFUSE_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
	float2 uv						: TEXCOORD0;
};

PS_ALPHA_TEST_DIFFUSE_INPUT OnlyAlpaTestWithDiffuseVS(VS_INPUT input)
{
	PS_ALPHA_TEST_DIFFUSE_INPUT output;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	output.position		= mul(posWorld,						onlyPass_viewProjMat);
	output.uv			= input.uv;

	return output;
}

float4 OnlyAlpaTestWithDiffusePS( PS_ALPHA_TEST_DIFFUSE_INPUT input ) : SV_TARGET
{ 
	float4 diffuse	= GetDiffuse(defaultSampler, input.uv);

	if(diffuse.a < ALPHA_TEST_BIAS)
		discard;

	return diffuse;
}