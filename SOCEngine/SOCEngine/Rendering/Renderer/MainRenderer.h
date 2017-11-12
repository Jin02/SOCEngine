#pragma once

#include "RenderTexture.h"
#include "DepthMap.h"
#include "OnlyLightCulling.h"
#include "ConstBuffer.h"
#include "MainCamera.h"
#include "ShaderManager.h"

#include "LightManager.h"
#include "ShadowManager.h"

#include "TileBasedShadingHeader.h"
#include "ComputeShader.h"
#include "ShadowAtlasMapRenderer.h"

namespace Rendering
{
	namespace Renderer
	{
		class MainRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera);
			void UpdateCB(Device::DirectX& dx, const Camera::MainCamera& mainCamera, const Manager::LightManager& lightMgr);

			struct Param
			{
				const Camera::MainCamera&				mainCamera;
				const MeshRenderer::Param&				renderParam;
				const Manager::MaterialManager&			materialMgr;
				const Manager::LightManager&			lightMgr;
				const Manager::ShadowManager&			shadowMgr;
				const Renderer::ShadowAtlasMapRenderer&	shadowRenderer;
			};
			void Render(Device::DirectX& dx, Param&& param);

			SET_ACCESSOR_DIRTY(Gamma,		float,			_gamma);
			GET_CONST_ACCESSOR(Gamma,		float,			_gamma);
			GET_CONST_ACCESSOR(GBuffers,	const auto&,	_gbuffer);
			GET_CONST_ACCESSOR(TBRParamCB,	const auto&,	_tbrCB);
			GET_ACCESSOR(ResultMap,			auto&,			_resultMap);

		private:
			Texture::RenderTexture		_resultMap;
			GBuffers					_gbuffer;

			TBRParamCB					_tbrCB;

			Shader::ComputeShader		_tbdrShader;
			Light::OnlyLightCulling		_blendedDepthLightCulling;

		private:
			TempRenderQueue								_renderQ;
			RenderQueue::TransparentMeshRenderQueue		_transparentMeshRenderQ;

		private:
			float						_gamma = 2.2f;
			bool						_dirty = true;
		};
	}

	struct MainRenderingSystemParam
	{
		Renderer::MainRenderer& renderer;
		Camera::MainCamera&		camera;

		MainRenderingSystemParam(Renderer::MainRenderer& _renderer, Camera::MainCamera& _camera)
			: camera(_camera), renderer(_renderer)
		{
		}
	};
}