#pragma once

#include "Mesh.h"
#include "Material.h"
#include "CameraForm.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyForm
		{
		public:
			enum class Type
			{
				Unknown,
				Box,
				Dome
			};

		private:
			Geometry::Mesh*				_mesh;

		public:
			SkyForm();
			virtual ~SkyForm();

		protected:
			void Initialize(const Material* skyMaterial);
			void _Render(const Device::DirectX* dx, const Texture::RenderTexture*& renderTarget, const Texture::DepthBuffer*& opaqueDepthBuffer);

		public:
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera, const Texture::RenderTexture*& renderTarget, const Texture::DepthBuffer*& opaqueDepthBuffer) = 0;
			virtual void Destroy();
		};
	}
}