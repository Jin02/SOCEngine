#pragma once

#include "RenderTexture.h"
#include "DepthBuffer.h"
#include "OnlyLightCulling.h"
#include "ConstBuffer.h"
#include "MainCamera.h"
#include "ShaderManager.h"

#include "LightManager.h"
#include "ShadowManager.h"

#include "TileBasedShadingHeader.h"

namespace Rendering
{
	namespace Renderer
	{
		class MainRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera);
			void UpdateCB(Device::DirectX& dx, const Camera::MainCamera& mainCamera, Manager::LightManager& lightMgr);

			GET_CONST_ACCESSOR(gamma, float, _tbrCBData.gamma);
			SET_ACCESSOR_DIRTY(Gamma, float, _tbrCBData.gamma);

			GET_ACCESSOR(GBuffers, auto&, _gbuffer);
			GET_ACCESSOR(TBRParamCB, auto&, _tbrCB);

		private:
			Texture::RenderTexture						_diffuseLightBuffer;
			Texture::RenderTexture						_specularLightBuffer;

			Light::OnlyLightCulling						_blendedDepthLC;

			Buffer::ExplicitConstBuffer<TBRCBData>		_tbrCB;
			TBRCBData									_tbrCBData;

			GBuffers									_gbuffer;
			bool										_dirty = true;
		};
	}
}