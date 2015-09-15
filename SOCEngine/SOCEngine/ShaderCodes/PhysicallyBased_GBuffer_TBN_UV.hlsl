#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 binormal			: BINORMAL;
};

struct VS_OUTPUT
{
	float4 position 	 	: SV_POSITION;
	float2 uv				: TEXCOORD0;

	float3 normal 			: NORMAL;
	float3 tangent 			: TANGENT;
	float3 binormal 		: BINORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

	ps.normal 		= normalize( mul(input.normal, (float3x3)transform_worldView ) );
	ps.tangent 		= normalize( mul(input.tangent, (float3x3)transform_worldView ) );
	ps.binormal 	= normalize( mul(input.binormal, (float3x3)transform_worldView ) );
 
    return ps;
}

float3 DecodeNormal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
	float3 texNormal = DecodeNormalTexture(normalTexture, uv, GBufferDefaultSampler);
	float3x3 TBN = float3x3(normalize(binormal), normalize(tangent), normalize(normal));

	return normalize( mul(texNormal, TBN) );
}

GBuffer PS( VS_OUTPUT input ) : SV_Target
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float opacityMap = 1.0f - opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap;
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	float metallic, roughness, emission;
	Parse_Metallic_Roughness_Emission(material_metallic_roughness_emission,
		metallic, roughness, emission);

	outGBuffer.albedo_emission.rgb		= diffuseTex.rgb * material_mainColor;
	outGBuffer.albedo_emission.a		= emission;

	outGBuffer.specular_metallic.rgb	= specularTexture.Sample(GBufferDefaultSampler, input.uv).rgb;
	outGBuffer.specular_metallic.a 		= metallic;

	outGBuffer.normal_roughness.rgb		= DecodeNormal(input.normal, input.tangent, input.binormal, input.uv) * 0.5f + 0.5f;
	outGBuffer.normal_roughness.a 		= roughness;

	return outGBuffer;
}