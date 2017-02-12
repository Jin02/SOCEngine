//USED_FOR_INCLUDE

struct PS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
};

PS_POSITION_ONLY_INPUT DepthOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	ps.position			= mul(posWorld,						onlyPass_viewProjMat);

	return ps;
}