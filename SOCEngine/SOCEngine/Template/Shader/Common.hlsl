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

cbuffer BasicMaterial : register( b0 )
{
	float3	material_mainColor;
	float 	material_opacity;
	float3 	material_specular;
	float 	material_shiness;
};


#define VS_CALC_DEPTH_WITH_POS(ps, inputPos) ps.position = mul(inputPos, transform_worldViewProj); float4 viewPos = mul(inputPos, transform_worldView);	ps.depth = viewPos.z / camera_far;
#define ALPHA_TEST 0.5f


//Linear Depth Buffer

struct DEPTH_WRITE_PS_INPUT
{
	float4	position 	: SV_POSITION;	
	float	depth 		: LINEAR_DEPTH;
};

DEPTH_WRITE_PS_INPUT DepthWriteVS(VS_INPUT input)
{
	DEPTH_WRITE_PS_INPUT ps;

	ps.position = mul( input.position, transform_worldViewProj );
	VS_CALC_DEPTH_WITH_POS(ps, input.position);

    return ps;
}

float4 DepthWritePS(DEPTH_WRITE_PS_INPUT input) : SV_Target
{
	float4 depth;
	depth.x	= input.depth;
	return depth;
}