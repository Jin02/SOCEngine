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

		public:
			void Initialize(const Camera::MeshCamera* meshCam, const Sky::SkyForm* sky);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT);
		};
	}
}