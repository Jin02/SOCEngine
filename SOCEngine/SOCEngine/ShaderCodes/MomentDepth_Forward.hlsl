//USED_FOR_INCLUDE

struct PS_MOMENT_DEPTH_INPUT
{
	float4	position 	 	: SV_POSITION;
	float	linearDepth		: DEPTH_IN_VIEW_SPACE;
};

PS_MOMENT_DEPTH_INPUT MomentDepthVS(VS_INPUT input)
{
	PS_MOMENT_DEPTH_INPUT ps;

	float4 posWorld		= mul(float4(input.position, 1.0f), transform_world);
	float4 posWorldView	= mul(posWorld, cameraMat_view);

	ps.linearDepth		= posWorldView.z / cameraOption_farZ;
	ps.position			= mul(posWorld, cameraMat_viewProj);

	return ps;
}

float2 MomentDepthPS(PS_MOMENT_DEPTH_INPUT input) : SV_TARGET
{
	float depth = input.linearDepth;

	float2 moment = float2(depth, depth * depth);

	float dx = ddx(depth);
	float dy = ddy(depth);

	moment.y += 0.25f * (dx * dx + dy * dy);

	return moment;
}