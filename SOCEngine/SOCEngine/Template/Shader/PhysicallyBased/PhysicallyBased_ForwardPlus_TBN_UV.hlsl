struct VS_INPUT
{
	float4 position					: POSITION;
	float2 uv						: TEXCOORD0;
	float3 normal					: NORMAL;
	float3 tangent					: TANGENT;
	float3 binormal					: BINORMAL;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;

	float2 uv						: TEXCOORD0;

	float3 normal 					: NORMAL;
	float3 tangent 					: TANGENT;
	float3 binormal 				: BINORMAL;
};

struct PS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
};

struct PS_ALPHA_TEST_WITH_UV_INPUT //used in alpha test
{
	float4 position 	 			: SV_POSITION;
	float2 uv						: TEXCOORD0;
};

Texture2D txDiffuse 				: register( t0 );
Texture2D txNormal	 				: register( t1 );
Texture2D txSpecular 				: register( t2 );
Texture2D txOpacity 				: register( t3 );

SamplerState samplerState 			: register( s0 );

/******************** Render Scene ********************/

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;
	ps.positionWorld	= mul(input.position, transform_world).xyz;
	ps.position			= mul(input.position, transform_worldViewProj);
	ps.uv				= input.uv;
	ps.normal			= mul(float4(input.normal, 0), transform_world).xyz;
	ps.tangent			= mul(float4(input.tangent, 0), transform_world).xyz;
	ps.binormal			= mul(float4(input.binormal, 0), transform_world).xyz;

	return ps;
}

float3 DecodeNormal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
	float3 tangentNormal = txNormal.Sample(samplerState, uv).xyz;
	tangentNormal = normalize( tangentNormal * 2 - 1 );

	float3x3 TBN = float3x3(normalize(tangent), normalize(binormal), normalize(normal));
	return mul(TBN, tangentNormal);
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float4 albedo = txDiffuse.Sample(samplerState, input.uv) * float4(material_mainColor, 1.0f);
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

float4 AlphaTestWithDiffusePS(PS_ALPHA_TEST_WITH_UV_INPUT input) : SV_Target
{
	float4 albedo = txDiffuse.Sample(samplerState, input.uv) * float4(material_mainColor, 1.0f);
	float alphaTest = albedo.a;

	if(alphaTest < ALPHA_TEST_BIAS)
		discard;

	return albedo;
}