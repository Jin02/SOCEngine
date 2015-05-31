struct VS_INPUT
{
	float4 position			: POSITION;
	float2 uv				: TEXCOORD0;
	float3 normal			: NORMAL;
};

struct PS_SCENE_INPUT
{
	float4 position 	 	: SV_POSITION;
	float3 positionWorld	: POSITION_WORLD;

	float2 uv				: TEXCOORD0;
	float3 normal 			: NORMAL;
};

struct PS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 position 	 	: SV_POSITION;
};

struct PS_ALPHA_TEST_WITH_UV_INPUT //used in alpha test
{
	float4 position 	 	: SV_POSITION;
	float2 uv				: TEXCOORD0;
};

Texture2D txDiffuse 		: register( t0 );
Texture2D txNormal	 		: register( t1 );
Texture2D txSpecular 		: register( t2 );
Texture2D txOpacity 		: register( t3 );

SamplerState sampler 		: register( s0 );

/******************** Render Scene ********************/

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;
	ps.positionWorld	= mul(input.position, transform_world);
	ps.position			= mul(input.position, transform_worldViewProj);
	ps.uv				= input.uv;
	ps.normal			= mul(input.normal, transform_world);\

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float4 albedo = txDiffuse.Sample(sampler, input.uv) * material_mainColor;
	return albedo;
}

/******************** Only Position, Only Write DepthBuffer ********************/

PS_POSITION_ONLY_INPUT PositionOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	ps.position = mul(input.position, transform_worldViewProj);

	return ps;
}

/******************** AlphaTest With DrawDiffuse ********************/

PS_ALPHA_TEST_WITH_UV_INPUT AlphaTestWithDiffuseVS( VS_INPUT input )
{
    PS_ALPHA_TEST_WITH_UV_INPUT ps;

	ps.position			= mul(input.position, transform_worldViewProj);
	ps.uv				= input.uv;

    return ps;
}

float4 AlphaTestWithDiffusePS(PS_ALPHA_TEST_WITH_UV_INPUT input)
{
	float4 albedo = txDiffuse.Sample(sampler, input.uv) * material_mainColor;
	float alphaTest = albedo.a;

	if(alphaTest < ALPHA_TEST_BIAS)
		discard;

	return albedo;
}