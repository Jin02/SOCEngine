#pragma once

#include "SkyGeometry.h"
#include "SkyBoxMaterial.h"
#include "ShaderManager.h"

#undef far

namespace Rendering
{
	namespace Sky
	{
		class SkyBox
		{
		public:
			void Initialize(Device::DirectX& dx);
			void UpdateCB(Device::DirectX& dx, const Math::Vector3& worldPos, const Math::Matrix& viewProjMat, float far);
			void Render(Device::DirectX& dx, Texture::RenderTexture& target, const Texture::DepthMap& targetDepthMap, const Material::SkyBoxMaterial& material);

		private:
			SkyGeometry									_geometry;
			Buffer::ExplicitConstBuffer<Math::Matrix>	_transformCB;
		};
	}
}