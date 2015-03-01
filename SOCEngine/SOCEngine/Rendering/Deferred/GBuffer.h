#pragma once

#include "DirectX.h"
#include "RenderTexture.h"
#include "ConstBuffer.h"
#include "RenderManager.h"

namespace Rendering
{
	namespace Deferred
	{
		class GBuffer
		{
		private:
			Texture::RenderTexture	*_albedo_opacity;
			Texture::RenderTexture	*_specular_fresnel0;
			Texture::RenderTexture	*_normal_roughness;
			Texture::RenderTexture	*_linearDepth;

		public:
			GBuffer();
			~GBuffer();

		public:
			void Init();
			void Destroy();
			void Clear(const Device::DirectX* dx);
			void Render(const Device::DirectX* dx, const Buffer::ConstBuffer* _cameraConstBuffer, const Manager::RenderManager* renderMgr);
		};
	}
};