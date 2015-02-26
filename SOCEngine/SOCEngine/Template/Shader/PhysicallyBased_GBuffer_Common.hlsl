//NOT_CREATE_META_DATA

cbuffer Transform : register( b0 )
{
	matrix transform_world;
	matrix transform_worldView;
	matrix transform_worldViewProj;
};

cbuffer Camera : register( b1 )
{
	float4 	camera_pos;
	float 	camera_near;
	float 	camera_far;
	float2 	camera_screenSize;
};

cbuffer MicrofacetBRDFParameter : register( b2 )
{
	float microfacetBRDF_roughness 	: packoffset(c0);
	float microfacetBRDF_fresnel0	: packoffset(c0.y);
};

struct GBuffer
{
	float4 albedo 				: SV_Target0;
	float4 specular_fresnel0	: SV_Target1;
	float4 normal_roughness 	: SV_Target2;
	float  depth 				: SV_Target3;
};

#define ALPHA_TEST_COMP_VALUE 0.3f

float3 ComputeFaceNormal(float3 position)
{
    return cross(ddx_coarse(position), ddy_coarse(position));
}