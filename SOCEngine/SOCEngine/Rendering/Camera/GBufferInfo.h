#pragma once

#include "RenderTexture.h"

struct GBuffers
{
	Rendering::Texture::RenderTexture*	albedo_occlusion;
	Rendering::Texture::RenderTexture*	normal_roughness;
	Rendering::Texture::RenderTexture*	motionXY_metallic_specularity;
	Rendering::Texture::RenderTexture*	emission_materialFlag;
	Rendering::Texture::DepthBuffer*	opaqueDepthBuffer;
	Rendering::Texture::DepthBuffer*	blendedDepthBuffer;
	
	GBuffers()	:	albedo_occlusion(nullptr), normal_roughness(nullptr), motionXY_metallic_specularity(nullptr),
					emission_materialFlag(nullptr), opaqueDepthBuffer(nullptr), blendedDepthBuffer(nullptr)
	{}
	~GBuffers(){}
};
