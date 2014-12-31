//NOT_CREATE_META_DATA

cbuffer Transform : register( b0 )
{
	matrix world;
	matrix worldView;
	matrix worldViewProj;
};

cbuffer Camera : register( b1 )
{
	float4 cameraPos;
	float clippingNear;
	float clippingFar;
	float2 screenSize;
};

cbuffer Material : register( b2 )
{
	float3 material_diffuse;
	float3 material_ambient;
	float3 material_specular;
	float3 materail_emissive;
	float shiness;
	float opacity;		
};