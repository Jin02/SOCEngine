#pragma once

#include "SkyGeometry.h"
#include "SkyBoxMaterial.h"
#include "ShaderManager.h"

#include "TileBasedShadingHeader.h"

#undef far

namespace Rendering
{
	namespace Renderer
	{
		class SkyBoxRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr);
			void Destroy();

			void UpdateCB(Device::DirectX& dx, const Math::Vector3& worldPos, const Math::Matrix& viewProjMat, float far);
			void Render(Device::DirectX& dx, Texture::RenderTexture& target, const Texture::DepthMap& targetDepthMap, const Material::SkyBoxMaterial& material, const Renderer::TBRParamCB& tbrParamCB);

		private:
			Sky::SkyGeometry							_geometry;
			Buffer::ExplicitConstBuffer<Math::Matrix>	_transformCB;
		};
	}
}