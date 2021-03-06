//NOT_CREATE_META_DATA

#ifndef __SOC_TBDR_INPUT_H__
#define __SOC_TBDR_INPUT_H__

#include "../GlobalDefine.h"

Buffer<float4>								PointLightTransformBuffer					: register( t0 ); // center, radius
Buffer<float4>								PointLightColorBuffer						: register( t1 ); // rgb, lumen(maximum : 30,000)

Buffer<float4>								SpotLightTransformBuffer					: register( t2 ); // center, radiusWithDirZSignBit
Buffer<float4>								SpotLightColorBuffer						: register( t3 ); // rgb, lumen(maximum : 30,000)
Buffer<float4>								SpotLightParamBuffer						: register( t4 ); // all half type(2byte) / dirXY, innerConeCosAngle, outerConeCosAngle

Buffer<float2>								DirectionalLightDirXYBuffer					: register( t5 ); // DirXY
Buffer<float4>								DirectionalLightColorBuffer					: register( t6 ); // rgb, lumen(maximum : 30,000)

#if (MSAA_SAMPLES_COUNT > 1)

Texture2DMS<float4, MSAA_SAMPLES_COUNT>		GBufferAlbedo_occlusion						: register( t8 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		GBufferVelocity_metallic_specularity		: register( t9 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		GBufferNormal_roughness						: register( t10 );
Texture2DMS<float,	MSAA_SAMPLES_COUNT>		GBufferDepth								: register( t11 );
Texture2DMS<uint4, MSAA_SAMPLES_COUNT>		GBufferEmission_materialFlag				: register( t31 );

#if defined(ENABLE_BLEND)
Texture2DMS<float,  MSAA_SAMPLES_COUNT>		GBufferBlendedDepth							: register( t12 );
#endif

#else //Turn off MSAA

Texture2D<float4>							GBufferAlbedo_occlusion						: register( t8 );
Texture2D<float4>							GBufferVelocity_metallic_specularity		: register( t9 );
Texture2D<float4>							GBufferNormal_roughness						: register( t10 );
Texture2D<float>							GBufferDepth								: register( t11 );
Texture2D<uint4>							GBufferEmission_materialFlag				: register( t31 );

#if defined(ENABLE_BLEND)
Texture2D<float>							GBufferBlendedDepth		 					: register( t12 );
#endif

#endif

//Buffer<uint> g_perLightIndicesInTile	: register( t13 ); -> in PhysicallyBased_Forward_Common.h

Buffer<uint4>								PointLightShadowParams						: register( t14 ); 
Buffer<uint4>								SpotLightShadowParams						: register( t15 ); 
Buffer<uint4>								DirectionalLightShadowParams				: register( t16 ); 

Texture2D<float>							PointLightShadowMapAtlas					: register( t17 );
Texture2D<float>							SpotLightShadowMapAtlas						: register( t18 );
Texture2D<float>							DirectionalLightShadowMapAtlas				: register( t19 );

Buffer<uint>								PointLightOptionalParamIndex				: register( t20 );
Buffer<uint>								SpotLightOptionalParamIndex					: register( t21 );
Buffer<uint>								DirectionalLightOptionalParamIndex			: register( t22 );

struct DSLightVPMat	{	matrix mat;		};
struct PLightVPMat	{	matrix mat[6];	};

StructuredBuffer<PLightVPMat>				PointLightShadowViewProjMatrix				: register( t23 );
StructuredBuffer<DSLightVPMat>				SpotLightShadowViewProjMatrix				: register( t24 );
StructuredBuffer<DSLightVPMat>				DirectionalLightShadowViewProjMatrix		: register( t25 );

Texture2D<float>							DirectionalLightShadowViewDepthMap			: register( t26 );

uint GetShadowIndex(uint lightOptionalParamIndex)
{
	return lightOptionalParamIndex >> 16;
}

uint GetLightFlag(uint lightOptionalParamIndex)
{
	return (lightOptionalParamIndex & 0x0000ff00) >> 8;
}

uint GetLightUserFlag(uint lightOptionalParamIndex)
{
	return (lightOptionalParamIndex & 0xff); 
}

float GetSignDirectionalLightDirZSign(uint directionalLightOptionalParamIndex)
{
	uint isMinus = GetLightFlag(directionalLightOptionalParamIndex) & 0x1;
	return float( 1.0f - 2.0f * float(isMinus) );
}

float3 GetDirectionalLightDir(uint lightIndex)
{
	float3 lightDir = float3(DirectionalLightDirXYBuffer[lightIndex], 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y) * GetSignDirectionalLightDirZSign(DirectionalLightOptionalParamIndex[lightIndex]);

	return lightDir;
}

#endif
