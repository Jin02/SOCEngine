#pragma once

#include "FullScreen.h"
#include "MeshCamera.h"
#include "SkyForm.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class GaussianBlur
		{
		private:
			Math::Size<uint>		_filteringSize;

			FullScreen*				_vertical;
			FullScreen*				_horizontal;
			Texture::RenderTexture*	_tempBuffer;

		public:
			GaussianBlur();
			virtual ~GaussianBlur();

		private:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT){}

		public:
			void Initialize(const Math::Size<uint>& size, DXGI_FORMAT format);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT, const Texture::RenderTexture* inputColorMap);
			void Destroy();
		};
	}
}