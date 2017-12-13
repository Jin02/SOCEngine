//USED_FOR_INCLUDE

struct GS_ALPHA_TEST_DIFFUSE_INPUT //used in writing depth buffer
{
	float4 worldPos : WORLD_POS;
	float2 uv		: TEXCOORD0;
};

GS_ALPHA_TEST_DIFFUSE_INPUT OnlyAlpaTestWithDiffuseVS(VS_INPUT input)
{
	GS_ALPHA_TEST_DIFFUSE_INPUT output;
	output.worldPos = mul(float4(input.position, 1.0f), transform_world);
	output.uv		= input.uv;

	return output;
}

struct PS_ALPHA_TEST_DIFFUSE_INPUT
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;

	uint rtIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void OnlyAlpaTestWithDiffuseGS(triangle GS_ALPHA_TEST_DIFFUSE_INPUT input[3], inout TriangleStream<PS_ALPHA_TEST_DIFFUSE_INPUT> stream)
{
    for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
		PS_ALPHA_TEST_DIFFUSE_INPUT output;
		output.rtIndex = faceIdx;

		for(uint i=0; i<3; ++i)
		{
			output.position	= mul(input[i].worldPos, rpInfo_viewProjs[faceIdx]);
			output.uv		= input[i].uv;

			stream.Append(output);
		}
		stream.RestartStrip();
    }
}

float4 OnlyAlpaTestWithDiffusePS( PS_ALPHA_TEST_DIFFUSE_INPUT input ) : SV_TARGET
{ 
	float4 diffuse	= GetDiffuse(defaultSampler, input.uv);

	if(diffuse.a < ALPHA_TEST_BIAS)
		discard;

	return diffuse;
}