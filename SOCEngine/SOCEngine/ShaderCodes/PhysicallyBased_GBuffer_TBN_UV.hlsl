#include "PhysicallyBased_GBuffer_Common.h"

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
	float3 texNormal = DecodeNormalTexture(normalTexture, uv, GBufferDefaultSampler);
	float3x3 TBN = float3x3(normalize(binormal), normalize(tangent), normalize(normal));

	return normalize( mul(texNormal, TBN) );
}

GBuffer PS( GEOMETRY_BUFFER_PS_INPUT input )
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float alpha = diffuseTex.a * opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	outGBuffer.albedo_metallic.rgb	= diffuseTex.rgb * material_mainColor;
	outGBuffer.albedo_metallic.a	= material_metallic;

	outGBuffer.specular_fresnel0	= specularTexture.Sample(GBufferDefaultSampler, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	outGBuffer.normal_roughness.rgb = DecodeNormal(input.normal, input.tangent, input.binormal, input.uv) * 0.5f + 0.5f;
	outGBuffer.normal_roughness.a 	= material_roughness;

	return outGBuffer;
}