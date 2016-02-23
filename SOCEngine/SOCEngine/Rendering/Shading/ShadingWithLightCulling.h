#pragma once

#include "LightCulling.h"
#include "RenderTexture.h"

namespace Rendering
{
	namespace TBDR
	{
		class ShadingWithLightCulling : public Light::LightCulling
		{
		public:
			struct GBuffers
			{
				const Texture::RenderTexture* albedo_sunOcclusion;
				const Texture::RenderTexture* specular_metallic;
				const Texture::RenderTexture* normal_roughness;
			};

		private:
			Rendering::Texture::RenderTexture*				_offScreen;

		public:
			ShadingWithLightCulling();
			virtual ~ShadingWithLightCulling();

		public:
			void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const GBuffers& geometryBuffers, const Math::Size<uint>& backBufferSize, bool useDebugMode = false);
			void Destory();

			void Dispatch(const Device::DirectX* dx, const Buffer::ConstBuffer* tbrConstBuffer, const Buffer::ConstBuffer* shadowGlobalParamConstBuffer);

		public:
			GET_ACCESSOR(UncompressedOffScreen, const Rendering::Texture::RenderTexture*, _offScreen);
		};
	}
}