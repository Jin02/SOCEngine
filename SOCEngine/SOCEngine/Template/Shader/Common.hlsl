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

cbuffer BasicMaterial : register( b2 )
{
	float3	material_mainColor;
	float 	material_opacity;
	float3 	material_specular;
	float 	material_shiness;
};

struct DEPTH_WRITE_PS_INPUT
{
	float4 pos 		: SV_POSITION;	
	float depth 	: LINEAR_DEPTH;
};

#define VS_CALC_DEPTH_WITH_POS(ps, inputPos) ps.pos = mul(inputPos, transform_worldViewProj); float4 viewPos = mul(inputPos, transform_worldView);	ps.depth = viewPos.z / camera_far;

#define ALPHA_TEST 0.5f

