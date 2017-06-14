#pragma once

#include "Matrix.h"
#include "Common.h"

#include "RenderTexture.h"
#include "DepthBuffer.h"

namespace Rendering
{
	namespace Renderer
	{
		struct TBRCBData
		{
			Math::Matrix 		invProjMat;
			Math::Matrix 		invViewProjMat;
			Math::Matrix 		invViewProjViewport;

			struct Packed
			{
				uint		packedViewportSize = 0;
				uint 		packedNumOfLights = 0;
				uint 		maxNumOfperLightInTile = 0;
			};

			Packed			packedParam;
			float			gamma = 2.2f;
		};

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
}