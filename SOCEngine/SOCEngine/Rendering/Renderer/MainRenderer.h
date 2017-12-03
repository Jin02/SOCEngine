#pragma once

#include <functional>

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

#include "SkyBox.h"
#include "GlobalIllumination.h"

#include "Copy.h"

namespace Rendering
{
	namespace Renderer
	{
		class MainRenderer final
		{
		public:
			MainRenderer() = default;
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera, const GlobalIllumination::InitParam&& giParam);
			void UpdateCB(Device::DirectX& dx, const Camera::MainCamera& mainCamera, const Manager::LightManager& lightMgr);

			struct Param
			{
				const Camera::MainCamera&				mainCamera;
				const MeshRenderer::Param&				renderParam;
				const Manager::MaterialManager&			materialMgr;
				const Manager::LightManager&			lightMgr;
				const ShadowSystem&&						shadowParam;
				const CullingParam&&					cullingParam;

				const Material::SkyBoxMaterial*			skyBoxMaterial;
			};
			void Render(Device::DirectX& dx, const Param&& param);

			SET_ACCESSOR_DIRTY(Gamma,		float,			_gamma);
			GET_CONST_ACCESSOR(Gamma,		float,			_gamma);
			GET_CONST_ACCESSOR(GBuffers,	const auto&,	_gbuffer);
			GET_CONST_ACCESSOR(TBRParamCB,	const auto&,	_tbrCB);
			GET_ACCESSOR_REF(ResultMap,						_resultMap);

		private:
			Texture::RenderTexture						_resultMap;
			Texture::RenderTexture						_scaledMap;

			GBuffers									_gbuffer;

			TBRParamCB									_tbrCB;

			Shader::ComputeShader						_tbdrShader;
			Light::OnlyLightCulling						_blendedDepthLightCulling;

			Sky::SkyBox									_skyBox;
			GlobalIllumination							_gi;
			PostProcessing::Copy						_mainRTBuilder;

		private:
			TempRenderQueue								_renderQ;
			RenderQueue::TransparentMeshRenderQueue		_transparentMeshRenderQ;

		private:
			Shader::ComputeShader::ThreadGroup			_tbdrThreadGroup;
			float										_gamma = 2.2f;
			bool										_dirty = true;
		};
	}

	struct MainRenderingSystemParam
	{
		const Renderer::MainRenderer&	renderer;
		const Camera::MainCamera&		camera;
	};
}