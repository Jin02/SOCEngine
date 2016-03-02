//USED_FOR_INCLUDE

float4 OnlyAlpaTestWithDiffusePS( PS_SCENE_INPUT input ) : SV_TARGET
{ 
	float4 diffuseTex	= diffuseMap.Sample(defaultSampler, input.uv);
	float opacityTex	= 1.0f - opacityMap.Sample(defaultSampler, input.uv).x;
	float alpha			= diffuseTex.a * opacityTex * GetMaterialMainColor().a;

	if(alpha < ALPHA_TEST_BIAS)
		discard;

	return float4(diffuseTex.rgb, alpha);
}