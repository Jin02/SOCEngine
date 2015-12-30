//NOT_CREATE_META_DATA

float4 OnlyAlpaTestWithDiffusePS( PS_SCENE_INPUT input ) : SV_TARGET
{ 
	float4 diffuseTex = diffuseTexture.Sample(defaultSampler, input.uv);
	float opacityMap = 1.0f - opacityTexture.Sample(defaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap * ParseMaterialAlpha();
	if(alpha < ALPHA_TEST_BIAS)
		discard;

	return float4(diffuseTex.rgb, alpha);
}