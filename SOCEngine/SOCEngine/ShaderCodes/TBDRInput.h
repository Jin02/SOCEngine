//NOT_CREATE_META_DATA

#ifndef __SOC_TBDR_INPUT_H__
#define __SOC_TBDR_INPUT_H__

#include "../GlobalDefine.h"

#define DEG_2_RAD(X) PI * X / 180.0f

Buffer<float4>								g_inputPointLightTransformBuffer					: register( t0 ); // center, radius
Buffer<float4>								g_inputPointLightColorBuffer						: register( t1 ); // rgb, lumen(maximum : 30,000)

Buffer<float4>								g_inputSpotLightTransformBuffer						: register( t2 ); // center, radiusWithDirZSignBit
Buffer<float4>								g_inputSpotLightColorBuffer							: register( t3 ); // rgb, lumen(maximum : 30,000)
Buffer<float4>								g_inputSpotLightParamBuffer							: register( t4 ); // all half type(2byte) / dirXY, innerConeCosAngle, outerConeCosAngle

Buffer<float4>								g_inputDirectionalLightTransformWithDirZBuffer		: register( t5 ); // center, DirZ
Buffer<float4>								g_inputDirectionalLightColorBuffer					: register( t6 ); // rgb, lumen(maximum : 30,000)
Buffer<float2>								g_inputDirectionalLightParamBuffer					: register( t7 ); // all half type(2byte) / dirX,  dirY

#if (MSAA_SAMPLES_COUNT > 1)

Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferAlbedo_sunOcclusion						: register( t8 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferSpecular_metallic							: register( t9 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferNormal_roughness							: register( t10 );
Texture2DMS<float,	MSAA_SAMPLES_COUNT>		g_tDepth											: register( t11 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferEmission									: register( t31 );

#if defined(ENABLE_BLEND)
Texture2DMS<float,  MSAA_SAMPLES_COUNT>		g_tBlendedDepth										: register( t12 );
#endif

#else //Turn off MSAA

Texture2D<float4>							g_tGBufferAlbedo_sunOcclusion						: register( t8 );
Texture2D<float4>							g_tGBufferSpecular_metallic							: register( t9 );
Texture2D<float4>							g_tGBufferNormal_roughness							: register( t10 );
Texture2D<float>							g_tDepth											: register( t11 );
Texture2D<float4>							g_tGBufferEmission									: register( t31 );

#if defined(ENABLE_BLEND)
Texture2D<float>							g_tBlendedDepth		 								: register( t12 );
#endif

#endif

//Buffer<uint> g_perLightIndicesInTile	: register( t13 ); -> in PhysicallyBased_Forward_Common.h

Buffer<uint3>								g_inputPointLightShadowParams						: register( t14 ); // lightIndexWithbiasWithFlag, color, underScanSize
Buffer<uint2>								g_inputSpotLightShadowParams						: register( t15 ); // lightIndexWithbiasWithFlag, color
Buffer<uint2>								g_inputDirectionalLightShadowParams					: register( t16 ); // lightIndexWithbiasWithFlag, color

Texture2D<float>							g_inputPointLightShadowMapAtlas						: register( t17 );
Texture2D<float>							g_inputSpotLightShadowMapAtlas						: register( t18 );
Texture2D<float>							g_inputDirectionalLightShadowMapAtlas				: register( t19 );

Buffer<uint>								g_inputPointLightShadowIndex						: register( t20 );
Buffer<uint>								g_inputSpotLightShadowIndex							: register( t21 );
Buffer<uint>								g_inputDirectionalLightShadowIndex					: register( t22 );

struct DSLightVPMat	{	matrix mat;		};
struct PLightVPMat	{	matrix mat[6];	};

StructuredBuffer<PLightVPMat>				g_inputPointLightShadowViewProjMatrix				: register( t23 );
StructuredBuffer<DSLightVPMat>				g_inputSpotLightShadowViewProjMatrix				: register( t24 );
StructuredBuffer<DSLightVPMat>				g_inputDirectionalLightShadowViewProjMatrix			: register( t25 );

Texture2D<float4>							g_inputPointLightMomentShadowMapAtlas				: register( t26 );
Texture2D<float4>							g_inputSpotLightMomentShadowMapAtlas				: register( t27 );
Texture2D<float4>							g_inputDirectionalLightMomentShadowMapAtlas			: register( t28 );

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

// b1, b2, b3은 PhysicallyBased_Common에 있음

cbuffer ShadowGlobalParam : register( b4 )
{	
	uint	shadowGlobalParam_packedNumOfShadowAtlasCapacity;
	uint	shadowGlobalParam_packedPowerOfTwoShadowResolution;
	uint	shadowGlobalParam_packedNumOfShadows;
	uint	shadowGlobalParam_dummy;
};

// b5, b6, b7 은 GI 관련

#endif