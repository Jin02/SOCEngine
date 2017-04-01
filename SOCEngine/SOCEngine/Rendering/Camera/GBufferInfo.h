#pragma once

#include "RenderTexture.h"
#include "DepthBuffer.h"

namespace Rendering
{
	struct GBuffers
	{
		Texture::RenderTexture		albedo_occlusion;
		Texture::RenderTexture		normal_roughness;
		Texture::RenderTexture		velocity_metallic_specularity;
		Texture::RenderTexture		emission_materialFlag;
		Texture::DepthBuffer		opaqueDepthBuffer;
		Texture::DepthBuffer		blendedDepthBuffer;
	};
}
