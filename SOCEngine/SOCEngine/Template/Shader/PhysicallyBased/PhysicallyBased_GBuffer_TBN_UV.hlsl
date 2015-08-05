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
	float2 uv				: TEXCOORD0;

	float3 normal 			: NORMAL;
	float3 tangent 			: TANGENT;
	float3 binormal 		: BINORMAL;
};

Texture2D txDiffuse 		: register( t0 );
Texture2D txNormal	 		: register( t1 );
Texture2D txSpecular 		: register( t2 );
Texture2D txOpacity 		: register( t3 );

SamplerState samplerState 		: register( s0 );

GEOMETRY_BUFFER_PS_INPUT VS( VS_INPUT input )
{
	GEOMETRY_BUFFER_PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

	ps.normal 		= normalize( mul(input.normal, (float3x3)transform_worldView ) );
	ps.tangent 		= normalize( mul(input.tangent, (float3x3)transform_worldView ) );
	ps.binormal 	= normalize( mul(input.binormal, (float3x3)transform_worldView ) );
 
    return ps;
}

float3 DecodeNormal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
	float3 tangentNormal = txNormal.Sample(samplerState, uv).xyz;
	tangentNormal = normalize( tangentNormal * 2 - 1 );

	float3x3 TBN = float3x3(normalize(tangent), normalize(binormal), normalize(normal));
	return mul(TBN, tangentNormal);
}

void PS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	float4 diffuseTex = txDiffuse.Sample(samplerState, input.uv);
#ifdef ENABLE_ALPHA_TEST
	if(diffuseTex.a < ALPHA_TEST_BIAS)
		discard;
#endif

#ifdef ENABLE_ALPHA_TEST
	outGBuffer.albedo 	= diffuseTex * material_mainColor;
	outGBuffer.albedo.a = 1;
#else
	outGBuffer.albedo 	= diffuseTex * material_mainColor;
#endif

	outGBuffer.specular_fresnel0	= txSpecular.Sample(samplerState, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	outGBuffer.normal_roughness.rgb = DecodeNormal(input.normal, input.tangent, input.binormal,  input.uv);
	outGBuffer.normal_roughness.a 	= material_roughness;
}