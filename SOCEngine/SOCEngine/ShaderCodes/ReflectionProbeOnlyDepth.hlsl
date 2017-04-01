//USED_FOR_INCLUDE

struct GS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 worldPos : WORLD_POS;
};

GS_POSITION_ONLY_INPUT DepthOnlyVS(VS_INPUT input)
{
	GS_POSITION_ONLY_INPUT output;
	output.worldPos = mul(float4(input.position, 1.0f), transform_world);

	return output;
}

struct GS_POSITION_ONLY_OUTPUT
{
	float4 position : SV_POSITION;
	uint rtIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void DepthOnlyGS(triangle GS_POSITION_ONLY_INPUT input[3], inout TriangleStream<GS_POSITION_ONLY_OUTPUT> stream)
{
    for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
		GS_POSITION_ONLY_OUTPUT output;
		output.rtIndex = faceIdx;

		for(uint i=0; i<3; ++i)
		{
			output.position = mul(input[i].worldPos, rpInfo_viewProjs[faceIdx]);
			stream.Append(output);
		}
		stream.RestartStrip();
    }
}