
cbuffer WVPMat : register(b0)
{
	matrix worldViewProjMat;
};

TextureCube	cubeMap		: register(t0);
SamplerState	linearSampler	: register(s0);

struct VS_INPUT
{
	float3 position 	: POSITION;
};

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 localPos		: LOCAL_POS;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = mul(float4(input.position, 1.0f), worldViewProjMat);
	output.localPos = input.position;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return cubeMap.Sample(linearSampler, input.localPos);
}
