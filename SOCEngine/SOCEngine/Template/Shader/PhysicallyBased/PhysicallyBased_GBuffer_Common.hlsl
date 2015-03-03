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

cbuffer Material : register( b2 )
{
	float3	material_mainColor;
	float	material_opacity;
	float 	material_roughness;
	float 	material_fresnel0;
	float2 	material_uvTiling;
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
