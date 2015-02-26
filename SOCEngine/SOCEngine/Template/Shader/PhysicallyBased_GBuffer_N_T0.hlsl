struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 tex				: TEXCOORD0;
	float3 normal			: NORMAL;
};

struct GEOMETRY_BUFFER_PS_INPUT
{
	float4 position 	: SV_Position;
	float3 positionView	: POSITION_VIEW; // View Space Position
	float3 normal 		: NORMAL;
	float2 tex			: TEXCOORD0;
	float  depth		: DEPTH;
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
	ps.normal 		= normalize( mul( input.normal, transform_worldView ) );
	ps.tex			= input.tex;
	ps.depth 		= ps.positionView.z / camera_far;
 
    return ps;
}

void PS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	outGBuffer.albedo 	= txDiffuse.Sample(sampler, input.uv);
	outGBuffer.albedo.a = 1.0f - txOpacity.Sample(sampler, input.uv);

	outGBuffer.specular_fresnel0	= txSpecular.Sample(sampler, input.uv);
	outGBuffer.specular_fresnel0.a 	= microfacetBRDF_fresnel0;

	outGBuffer.normal_roughness.rgb = input.normal;
	outGBuffer.normal_roughness.a 	= microfacetBRDF_roughness;

	outGBuffer.depth.r = input.depth;
}

void AlphaTestPS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	PS(input, outGBuffer);

	// Alpha Test
	clip(outGBuffer.albedo.a - ALPHA_TEST_COMP_VALUE);

	outGBuffer.normal_roughness.rgb = normalize( ComputeFaceNormal(input.positionView) );
}