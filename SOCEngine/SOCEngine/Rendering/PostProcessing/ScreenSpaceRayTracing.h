#pragma once

#include "FullScreen.h"
#include "MeshCamera.h"
#include "SkyForm.h"
#include "GBufferInfo.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class ScreenSpaceRayTracing : public FullScreen
		{
		public:
			enum class Type	{	Near, Far	};

		private:
			Buffer::ConstBuffer*		_viewToTexSpaceCB;
			Texture::RenderTexture*		_rayTracingBuffer;

		public:
			ScreenSpaceRayTracing();
			virtual ~ScreenSpaceRayTracing();

		private:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT){}

		public:
			void Initialize(const Device::DirectX* dx, const Buffer::ConstBuffer* ssrtParam);
			void UpdateConstBuffer(const Device::DirectX* dx);
			void Render(const Device::DirectX* dx, const GBufferInfo* gbuffer);
			void Destroy();
		};
	}
}
