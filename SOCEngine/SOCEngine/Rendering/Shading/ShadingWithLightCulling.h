#pragma once

#include "LightCulling.h"
#include "RenderTexture.h"
#include "SkyForm.h"

namespace Rendering
{
	namespace TBDR
	{
		class ShadingWithLightCulling : public Light::LightCulling
		{
		public:
			struct GBuffers
			{
				const Texture::RenderTexture* albedo_occlusion;
				const Texture::RenderTexture* motionXY_metallic_specularity;
				const Texture::RenderTexture* normal_roughness;
				const Texture::RenderTexture* emission_materialFlag;
			};

		private:
			Rendering::Texture::RenderTexture*	_diffuseLightBuffer;
			Rendering::Texture::RenderTexture*	_specularLightBuffer;

		public:
			ShadingWithLightCulling();
			virtual ~ShadingWithLightCulling();

		public:
			void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const GBuffers& geometryBuffers, const Math::Size<uint>& backBufferSize, bool useDebugMode = false);
			void Destory();

			void Dispatch(const Device::DirectX* dx, const Buffer::ConstBuffer* tbrConstBuffer, const Buffer::ConstBuffer* shadowGlobalParamConstBuffer);

		public:
			GET_ACCESSOR(DiffuseLightBuffer,	const Rendering::Texture::RenderTexture*,	_diffuseLightBuffer);
			GET_ACCESSOR(SpecularLightBuffer,	const Rendering::Texture::RenderTexture*,	_specularLightBuffer);
		};
	}
}