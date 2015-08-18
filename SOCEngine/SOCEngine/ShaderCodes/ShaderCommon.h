//NOT_CREATE_META_DATA

#ifndef __SOC_SHADER_COMMON_H__
#define __SOC_SHADER_COMMON_H__

#define PI								3.141592654f
#define FLOAT_MAX						3.402823466e+38F

#define TILE_RES 						16
#define TILE_RES_HALF					(TILE_RES / 2)
#define SHADING_THREAD_COUNT			(TILE_RES * TILE_RES)
#define LIGHT_CULLING_THREAD_COUNT 		(TILE_RES_HALF * TILE_RES_HALF)

#define LIGHT_MAX_COUNT_IN_TILE 		544


Buffer<float4> g_inputPointLightTransformBuffer						: register( t0 ); // center, radius
Buffer<float4> g_inputPointLightColorBuffer							: register( t1 ); // rgb, intensity

Buffer<float4> g_inputSpotLightTransformBuffer						: register( t2 ); // center, radius
Buffer<float4> g_inputSpotLightColorBuffer							: register( t3 ); // rgb, intensity
Buffer<float4> g_inputSpotLightParamBuffer							: register( t4 ); // all half type(2byte) / dirXY, coneCosAngle,  falloffWithDirZSignBit

Buffer<float4> g_inputDirectionalLightTransformWithDirZBuffer		: register( t5 ); // center, DirZ
Buffer<float4> g_inputDirectionalLightColorBuffer					: register( t6 ); // rgb, intensity
Buffer<float2> g_inputDirectionalLightParamBuffer					: register( t7 ); // all half type(2byte) / dirX,  dirY

#if (MSAA_SAMPLES_COUNT > 1)

Texture2DMS<float4, MSAA_SAMPLES_COUNT> g_tGBufferAlbedo_metallic	: register( t8 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT> g_tGBufferSpecular_fresnel0	: register( t9 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT> g_tGBufferNormal_roughness	: register( t10 );
Texture2DMS<float,	MSAA_SAMPLES_COUNT> g_tDepth					: register( t11 );

#if defined(ENABLE_BLEND)
Texture2DMS<float, MSAA_SAMPLES_COUNT>	g_tBlendedDepth				: register( t12 );
#endif

#else //Turn off MSAA

Texture2D<float4>	g_tGBufferAlbedo_metallic						: register( t8 );
Texture2D<float4>	g_tGBufferSpecular_fresnel0						: register( t9 );
Texture2D<float4>	g_tGBufferNormal_roughness						: register( t10 );
Texture2D<float>	g_tDepth										: register( t11 );

#if defined(ENABLE_BLEND)
Texture2D<float> 	g_tBlendedDepth		 							: register( t12 );
#endif

#endif

cbuffer TileBasedDeferredShadingParams								: register( b0 )
{
	matrix	g_invViewProjViewport;
	float3	g_cameraWorldPosition;
	uint	g_directionalLightCount;
};

struct LightingParams
{
	uint	lightIndex;
	float3	viewDir;
	float3	normal;
	float	fresnel0;
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

#endif