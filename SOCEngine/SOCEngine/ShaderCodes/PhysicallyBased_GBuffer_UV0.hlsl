#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float3 position 		: POSITION;
	float2 uv				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 	 	: SV_POSITION;
	float3 worldPos			: WORLD_POS;
	float2 uv				: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.worldPos 	= mul( float4(input.position, 1.0f),	transform_world );
	ps.position 	= mul( ps.worldPos,						camera_viewProjMat );

	ps.uv			= input.uv;

    return ps;
}

GBuffer PS( VS_OUTPUT input) : SV_Target
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float opacityMap = 1.0f - opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap * ParseMaterialAlpha();
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif
	float3 normal	= float3(0.f, 0.f, 0.f);
	float4 specular	= specularTexture.Sample(GBufferDefaultSampler, input.uv);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalTexture.Sample(GBufferDefaultSampler, input.uv).a;
	MakeGBuffer(diffuseTex, float4(normal, roughness), specular, input.worldPos,
		outGBuffer.albedo_sunOcclusion, outGBuffer.specular_metallic, outGBuffer.normal_roughness, outGBuffer.emission);
#else
	MakeGBuffer(diffuseTex, normal, specular, input.worldPos,
	outGBuffer.albedo_sunOcclusion, outGBuffer.specular_metallic, outGBuffer.normal_roughness, outGBuffer.emission);
#endif

	return outGBuffer;
}
