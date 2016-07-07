#pragma once

#include "FullScreen.h"
#include "MeshCamera.h"
#include "SkyForm.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class BilateralFiltering
		{
		public:
			enum class Type	{	Near, Far	};

		private:
			Math::Size<uint>		_filteringSize;

			FullScreen*				_vertical;
			FullScreen*				_horizontal;
			Texture::RenderTexture*	_tempBuffer;

		public:
			BilateralFiltering();
			virtual ~BilateralFiltering();

		private:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT){}

		public:
			void Initialize(Type type, const Math::Size<uint>& size, DXGI_FORMAT format);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT,
				const Texture::DepthBuffer* depthBuffer, const Texture::RenderTexture* inputColorMap);
		};
	}
}