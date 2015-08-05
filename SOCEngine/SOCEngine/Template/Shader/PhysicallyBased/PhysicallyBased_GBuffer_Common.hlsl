//NOT_CREATE_META_DATA

cbuffer Transform : register( b0 )		//Mesh
{
	matrix transform_world;
	matrix transform_worldView;
	matrix transform_worldViewProj;
};

cbuffer Camera : register( b1 )			//CameraForm
{
	float4 	camera_pos;
	float 	camera_near;
	float 	camera_far;
	float2 	camera_screenSize;
};

cbuffer Material : register( b2 )		//PhysicallyBasedMaterial
{
	float4	material_mainColor;
	float 	material_roughness;
	float 	material_fresnel0;
	float2 	material_uvTiling;
};

struct GBuffer
{
	float4 albedo 				: SV_Target0;
	float4 specular_fresnel0	: SV_Target1;
	float4 normal_roughness 	: SV_Target2;
};

#ifdef ENABLE_MSAA
#define ALPHA_TEST_BIAS 0.003f
#else
#define ALPHA_TEST_BIAS 0.5f
#endif

float3 ComputeFaceNormal(float3 position)
{
    return cross(ddx_coarse(position), ddy_coarse(position));
}
