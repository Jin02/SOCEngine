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
		public:
			struct ParamCB
			{
				float sigma;
				float numPixelPerSide;
				float blurSize;
				float scale;
			};

		private:
			FullScreen*				_vertical;
			FullScreen*				_horizontal;
			Texture::RenderTexture*	_tempMap;

			Buffer::ConstBuffer*	_paramCB;

		public:
			GaussianBlur();
			virtual ~GaussianBlur();

		private:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT){}

		public:
			void Initialize(const Device::DirectX* dx, const Math::Size<uint>& tempBufferSize, DXGI_FORMAT format);
			void Initialize(const Device::DirectX* dx);

			void UpdateParam(const Device::DirectX* dx, const ParamCB& param);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT, const Texture::RenderTexture* inputColorMap);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT, const Texture::RenderTexture* inputColorMap, const Texture::RenderTexture* tempMap);
			void Destroy();
		};
	}
}