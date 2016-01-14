//NOT_CREATE_META_DATA

#ifndef __SOC_SHADER_COMMON_H__
#define __SOC_SHADER_COMMON_H__

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

Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferAlbedo_emission							: register( t8 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferSpecular_metallic							: register( t9 );
Texture2DMS<float4, MSAA_SAMPLES_COUNT>		g_tGBufferNormal_roughness							: register( t10 );
Texture2DMS<float,	MSAA_SAMPLES_COUNT>		g_tDepth											: register( t11 );

#if defined(ENABLE_BLEND)
Texture2DMS<float,  MSAA_SAMPLES_COUNT>		g_tBlendedDepth										: register( t12 );
#endif

#else //Turn off MSAA

Texture2D<float4>							g_tGBufferAlbedo_emission							: register( t8 );
Texture2D<float4>							g_tGBufferSpecular_metallic							: register( t9 );
Texture2D<float4>							g_tGBufferNormal_roughness							: register( t10 );
Texture2D<float>							g_tDepth											: register( t11 );

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

Texture3D<float4>							g_inputAnistropicVoxelAlbedoTexture					: register( t29 );
Texture3D<float>							g_inputAnistropicVoxelNormalTexture					: register( t30 );
Texture3D<float4>							g_inputAnistropicVoxelEmissionTexture				: register( t31 );

StructuredBuffer<DSLightVPMat>				g_inputDirectionalLightShadowInvVPVMatBuffer		: register( t32 );
StructuredBuffer<PLightVPMat>				g_inputPointLightShadowInvVPVMatBuffer				: register( t33 );
StructuredBuffer<DSLightVPMat>				g_inputSpotLightShadowInvVPVMatBuffer				: register( t34 );


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

cbuffer Transform : register( b1 )		//Object World
{
	matrix transform_world;
};

cbuffer CameraMat : register( b2 )
{
	matrix	cameraMat_view;
	matrix	cameraMat_viewProj;
};

// b3은 Material

cbuffer ShadowGlobalParam : register( b4 )
{	
	uint	shadowGlobalParam_packedNumOfShadowAtlasCapacity;
	uint	shadowGlobalParam_packedPowerOfTwoShadowResolution;
	uint	shadowGlobalParam_packedNumOfShadows;
	uint	shadowGlobalParam_dummy;
};

// b5, b6, b7 은 GI 관련

uint GetNumOfPointLight(uint packedNumOfLights)
{
	return packedNumOfLights >> 21;
}

uint GetNumOfSpotLight(uint packedNumOfLights)
{
	return (packedNumOfLights >> 10) & 0x7FF;
}

uint GetNumOfDirectionalLight(uint packedNumOfLights)
{
	return packedNumOfLights & 0x000003FF;
}

float4 CreatePlaneNormal( float4 b, float4 c )
{
    float4 n;
    //b.xyz - a.xyz, c.xyz - a.xyz이다.
    //여기서, a는 원점이다. 즉, ab는 원점에서 해당 타일의 꼭짓점까지 떨어진 방향을 뜻한다.
    n.xyz = normalize(cross( b.xyz, c.xyz ));
    n.w = 0;

    return n;
}

bool InFrustum( float4 p, float4 frusutmNormal, float r )
{
	//여기서 뒤에 + frusutmNormal.w 해야하지만, 이 값은 0이라 더할 필요 없음
	return (dot( frusutmNormal.xyz, p.xyz )/*+ frusutmNormal.w*/ < r);
}

uint Float4ToUint(float4 value)
{
	value *= 255.0f;

	uint4 ret;
	ret.x = (uint(value.x) & 0x000000FF);
	ret.y = (uint(value.y) & 0x000000FF) << 8;
	ret.z = (uint(value.z) & 0x000000FF) << 16;
	ret.w = (uint(value.w) & 0x000000FF) << 24;

	return ret.w | ret.z | ret.y | ret.x;
}

float4 UintToFloat4(uint value)
{
	float4 ret;
	ret.x = float( value & 0x000000FF);
	ret.y = float((value & 0x0000FF00) >> 8);
	ret.z = float((value & 0x00FF0000) >> 16);
	ret.w = float((value & 0xFF000000) >> 24);

	return (ret / 255.0f);
}

float smoothStep(float low, float high, float v)
{
	return clamp( (v - low) / (high - low), 0.0f, 1.0f);
}

struct ParsedShadowParam
{
	uint	lightIndex;
	uint	flag;
	float	bias;
	float4	color;
};

void ParseShadowParam(out ParsedShadowParam outParam, uint2 shadowParam)
{
	outParam.lightIndex	= shadowParam.x >> 16;
	outParam.bias		= float(uint(shadowParam.x & 0xfff0) >> 4) / (8192.0f);
	outParam.flag		= shadowParam.x & 0xf;
	outParam.color		= UintToFloat4(shadowParam.y);
}

#endif