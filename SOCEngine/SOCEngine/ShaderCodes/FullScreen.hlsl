struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float2 uv					: TEXCOORD0;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	uint sampleIdx				: SV_SAMPLEINDEX;
#endif
};

// id는 0, 1, 2 3개의 값만 들어온다.
PS_INPUT VS(uint id : SV_VERTEXID)
{
	PS_INPUT ps = (PS_INPUT)0;

	ps.position.x = (float)(id / 2) * 4.0f - 1.0f;
	ps.position.y = (float)(id % 2) * 4.0f - 1.0f;
	ps.position.z = 1.0f; //inverted depth
	ps.position.w = 1.0f;

	ps.uv.x = (float)(id / 2) * 2.0f;
	ps.uv.y = 1.0f - (float)(id % 2) * 2.0f;

	return ps;
}

SamplerState samplerState		: register( s0 );

#if (MSAA_SAMPLES_COUNT > 1)
Texture2DMS<float4,MSAA_SAMPLES_COUNT>	offScreenBuffer	: register( t0 );
#else
Texture2D<float4>						offScreenBuffer	: register( t0 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
#if (MSAA_SAMPLES_COUNT > 1)
	int2 pos = int2(input.position.xy);
	return offScreenBuffer.Load(pos.xy, input.sampleIdx);
#else
	return offScreenBuffer.SampleLevel(samplerState, input.uv, 0);
#endif
}