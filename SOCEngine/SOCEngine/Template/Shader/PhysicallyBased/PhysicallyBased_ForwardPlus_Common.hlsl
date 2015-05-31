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

#ifdef(MSAA_ENABLE)
#define ALPHA_TEST_BIAS 0.003f
#else
#define ALPHA_TEST_BIAS 0.5f
#endif