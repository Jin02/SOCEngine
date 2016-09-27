#pragma once

#include "RenderTexture.h"

namespace Rendering
{
	struct GBuffers
	{
		Texture::RenderTexture*		albedo_occlusion;
		Texture::RenderTexture*		normal_roughness;
		Texture::RenderTexture*		motionXY_metallic_specularity;
		Texture::RenderTexture*		emission_materialFlag;
		Texture::DepthBuffer*		opaqueDepthBuffer;
		Texture::DepthBuffer*		blendedDepthBuffer;
	
		GBuffers() :	albedo_occlusion(nullptr), normal_roughness(nullptr), motionXY_metallic_specularity(nullptr),
				emission_materialFlag(nullptr), opaqueDepthBuffer(nullptr), blendedDepthBuffer(nullptr) {}
		~GBuffers(){}
	};
}
