#pragma once

#include "Material.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyBoxMaterial : public Material
		{
		private:
			Buffer::ConstBuffer*	_wvpConstBuffer;

		public:
			SkyBoxMaterial(const std::string& name);
			virtual ~SkyBoxMaterial();

		public:
			virtual void Initialize();
			virtual void Destroy();

			void UpdateCubeMap(const Rendering::Texture::Texture2D* tex);
			void UpdateWVPMatrix(const Device::DirectX* dx, const Math::Matrix& transposedWVPMat);
		};
	}
}