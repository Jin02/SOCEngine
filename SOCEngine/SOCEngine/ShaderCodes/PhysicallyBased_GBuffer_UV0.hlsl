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

	float4 worldPos	= mul( float4(input.position, 1.0f),	transform_world );
	ps.position 	= mul( worldPos,						camera_viewProjMat );
	ps.worldPos		= worldPos.xyz;

	ps.uv			= input.uv;

    return ps;
}

GBuffer PS( VS_OUTPUT input) : SV_Target
{
	GBuffer outGBuffer;

#ifdef ENABLE_ALPHA_TEST
	float4 diffuseTex	= diffuseMap.Sample(GBufferDefaultSampler, input.uv);
	float opacityTex	= 1.0f - opacityMap.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha			= diffuseTex.a * opacityTex * GetMaterialMainColor().a;

	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	float3 normal	= float3(0.f, 0.f, 0.f);
	MakeGBuffer(normal, input.uv, outGBuffer.albedo_occlusion, outGBuffer.motionXY_height_metallic, outGBuffer.normal_roughness, outGBuffer.emission_specularity);

	return outGBuffer;
}
