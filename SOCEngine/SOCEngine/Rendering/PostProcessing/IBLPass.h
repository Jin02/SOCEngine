#pragma once

#include "FullScreen.h"
#include "MeshCamera.h"
#include "SkyForm.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class IBLPass : public FullScreen
		{
		public:
			IBLPass();
			virtual ~IBLPass();

		private:
			virtual void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT){}

		public:
			void Initialize(const Sky::SkyForm* sky);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT, const Camera::MeshCamera* meshCam);
		};
	}
}