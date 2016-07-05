#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float3 position 		: POSITION;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float2 uv			: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 	 	: SV_POSITION;
	float2 uv			: TEXCOORD0;
	float3 worldPos			: WORLD_POS;

	float3 normal 			: NORMAL;
	float3 tangent 			: TANGENT;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	float4 worldPos	= mul( float4(input.position, 1.0f),	transform_world );
	ps.position 	= mul( worldPos,			camera_viewProjMat );
	ps.worldPos	= worldPos.xyz;

	ps.normal 	= normalize( mul(input.normal, (float3x3)transform_worldInvTranspose ) );
	ps.tangent 	= normalize( mul(input.tangent, (float3x3)transform_worldInvTranspose ) );
 
    return ps;
}

GBuffer PS( VS_OUTPUT input ) : SV_Target
{
	GBuffer outGBuffer;

#ifdef ENABLE_ALPHA_TEST
	float4 diffuseTex	= DiffuseMap.Sample(GBufferDefaultSampler, input.uv);
	float opacityTex	= 1.0f - OpacityMap.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha		= diffuseTex.a * opacityTex * GetMaterialMainColor().a;

	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	float4 normalTex	= NormalMap.Sample(GBufferDefaultSampler, input.uv);
	float3 bumpedNormal	= NormalMapping(normalTex.rgb, input.normal, input.tangent, input.uv);
	float3 normal		= normalize(lerp(input.normal, bumpedNormal, HasNormalMap()));

	MakeGBuffer(normal, input.uv, outGBuffer.albedo_occlusion, outGBuffer.motionXY_height_metallic, outGBuffer.normal_roughness, outGBuffer.emission_specularity);

	return outGBuffer;
}
