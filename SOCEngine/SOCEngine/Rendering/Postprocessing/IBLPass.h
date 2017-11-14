#pragma once

#include "FullScreen.h"
#include "MainRenderer.h"
#include "MainCamera.h"
#include "PreIntegrateEnvBRDF.h"

#include "PPCommon.h"

namespace Rendering
{
	namespace Material
	{
		class SkyBoxMaterial;
	}

	namespace PostProcessing
	{
		class IBLPass final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT,
						const MainRenderingSystemParam&& mains, const Material::SkyBoxMaterial& skyBox) const;

		private:
			FullScreen						_screen;
			Precompute::PreIntegrateEnvBRDF	_envBRDFMap;
		};
	}
}