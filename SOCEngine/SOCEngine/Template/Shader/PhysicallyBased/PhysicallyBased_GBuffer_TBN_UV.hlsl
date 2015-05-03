struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 binormal			: BINORMAL;
};

struct GEOMETRY_BUFFER_PS_INPUT
{
	float4 position 	 	: SV_POSITION;
	float3 positionView		: POSITION_VIEW; // View Space Position
	float2 uv				: TEXCOORD0;

	float3 normal 			: NORMAL;
	float3 tangent 			: TANGENT;
	float3 binormal 		: BINORMAL;
};

Texture2D txDiffuse 		: register( t0 );
Texture2D txNormal	 		: register( t1 );
Texture2D txSpecular 		: register( t2 );
Texture2D txOpacity 		: register( t3 );

SamplerState sampler 		: register( s0 );

GEOMETRY_BUFFER_PS_INPUT VS( VS_INPUT input )
{
	GEOMETRY_BUFFER_PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.positionView = mul( input.position, transform_worldView );
	ps.uv			= input.uv;

	ps.normal 		= normalize( mul( input.normal, transform_worldView ) );
	ps.tangent 		= normalize( mul( input.tangent, transform_worldView ) );
	ps.binormal 	= normalize( mul( input.binormal, transform_worldView ) );
 
    return ps;
}

float3 DecodeNormal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
	float3 tangentNormal = txNormal.Sample(sampler, uv).xyz;
	tangentNormal = normalize( tangentNormal * 2 - 1 );

	float3x3 TBN = float3x3(normalize(tangent), normalize(binormal), normalize(normal));
	return mul(TBN, tangentNormal);
}

void PS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	outGBuffer.albedo 	= txDiffuse.Sample(sampler, input.uv) * material_mainColor;

	outGBuffer.specular_fresnel0	= txSpecular.Sample(sampler, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	outGBuffer.normal_roughness.rgb = DecodeNormal(input.normal, input.tangent, input.binormal,  input.uv);
	outGBuffer.normal_roughness.a 	= material_roughness;
}

void AlphaTestPS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	PS(input, outGBuffer);
	
	outGBuffer.albedo.a = (1.0f - txOpacity.Sample(sampler, input.uv)) * material_opacity;
	
	// Alpha Test
	clip(outGBuffer.albedo.a - ALPHA_TEST_COMP_VALUE);

	outGBuffer.normal_roughness.rgb = normalize( ComputeFaceNormal(input.positionView) );
}