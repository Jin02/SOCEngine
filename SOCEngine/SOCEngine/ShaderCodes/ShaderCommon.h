//NOT_CREATE_META_DATA

#ifndef __SOC_SHADER_COMMON_H__
#define __SOC_SHADER_COMMON_H__

#define PI								3.141592654f
#define FLOAT_MAX						3.402823466e+38F
#define DEG_2_RAD(X)					PI * X / 180.0f

Buffer<float4> g_inputPointLightTransformBuffer						: register( t0 ); // center, radius
Buffer<float4> g_inputPointLightColorBuffer							: register( t1 ); // rgb, intensity

Buffer<float4> g_inputSpotLightTransformBuffer						: register( t2 ); // center, radius
Buffer<float4> g_inputSpotLightColorBuffer							: register( t3 ); // rgb, intensity
Buffer<float4> g_inputSpotLightParamBuffer							: register( t4 ); // all half type(2byte) / dirXY, coneCosAngle,  falloffWithDirZSignBit

Buffer<float4> g_inputDirectionalLightTransformWithDirZBuffer		: register( t5 ); // center, DirZ
Buffer<float4> g_inputDirectionalLightColorBuffer					: register( t6 ); // rgb, intensity
Buffer<float2> g_inputDirectionalLightParamBuffer					: register( t7 ); // all half type(2byte) / dirX,  dirY

#if (MSAA_SAMPLES_COUNT > 1)

Texture2DMS<float4, MSAA_SAMPLES_COUNT> g_tGBufferAlbedo_emission	: register( t8 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT> g_tGBufferNormal_roughness	: register( t9 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT> g_tGBufferSpecular_metallic	: register( t10 );
Texture2DMS<float,	MSAA_SAMPLES_COUNT> g_tDepth					: register( t11 );

#if defined(ENABLE_BLEND)
Texture2DMS<float, MSAA_SAMPLES_COUNT>	g_tBlendedDepth				: register( t12 );
#endif

#else //Turn off MSAA

Texture2D<float4>	g_tGBufferAlbedo_emission						: register( t8 );
Texture2D<float4>	g_tGBufferSpecular_metallic						: register( t9 );
Texture2D<float4>	g_tGBufferNormal_roughness						: register( t10 );
Texture2D<float>	g_tDepth										: register( t11 );

#if defined(ENABLE_BLEND)
Texture2D<float> 	g_tBlendedDepth		 							: register( t12 );
#endif

#endif

struct LightingParams
{
	uint	lightIndex;
	float3	viewDir;
	float3	normal;
	float	roughness;
	float3	diffuseColor;
	float3	specularColor;
};

struct LightingCommonParams
{
	float3	lightColor;
	float	lightIntensity;
	float3	lightDir;
};

cbuffer TBRParam : register( b0 )
{
	matrix	tbrParam_viewMat;
	matrix 	tbrParam_invProjMat;
	matrix	tbrParam_invViewProjViewportMat;

	float2	tbrParam_viewPortSize;
	uint 	tbrParam_packedNumOfLights;
	uint	tbrParam_maxNumOfPerLightInTile;
	
	float4	tbrParam_cameraWorldPosition;
};

#endif