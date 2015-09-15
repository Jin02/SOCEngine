#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 	 	: SV_POSITION;
	float3 positionView		: POSITION_VIEW; // View Space Position
	float2 uv				: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

    return ps;
}

GBuffer PS( VS_OUTPUT input) : SV_Target
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float opacityMap = 1.0f - opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap;
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif
	float3 normal	= float3(0.f, 0.f, 0.f);
	float4 specular	= specularTexture.Sample(GBufferDefaultSampler, input.uv);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalTexture.Sample(GBufferDefaultSampler, input.uv).a;
	MakeGBuffer(diffuseTex, float4(normal, roughness), specular, outGBuffer.albedo_emission, outGBuffer.specular_metallic, outGBuffer.normal_roughness);
#else
	MakeGBuffer(diffuseTex, normal, specular, outGBuffer.albedo_emission, outGBuffer.specular_metallic,	outGBuffer.normal_roughness);
#endif

	return outGBuffer;
}
