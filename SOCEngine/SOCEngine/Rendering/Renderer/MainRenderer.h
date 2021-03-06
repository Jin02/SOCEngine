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

#include "SkyBoxRenderer.h"
#include "GlobalIllumination.h"

#include "MainSceneMaker.h"
#include "PreIntegrateEnvBRDF.h"

namespace Rendering
{
	namespace Renderer
	{
		class MainRenderer final
		{
		public:
			MainRenderer() = default;
			DISALLOW_ASSIGN_COPY(MainRenderer);

			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, Manager::BufferManager& bufferMgr, const Camera::MainCamera& mainCamera, const GIInitParam& giParam);
			void Destroy();

			void UpdateCB(Device::DirectX& dx, const Camera::MainCamera& mainCamera, const Manager::LightManager& lightMgr);

			struct Param
			{
				const Camera::MainCamera&				mainCamera;
				const MeshRenderer::Param&				renderParam;
				const Manager::MaterialManager&			materialMgr;
				const Manager::LightManager&			lightMgr;
				const ShadowSystem&&					shadowParam;
				const CullingParam&&					cullingParam;

				const Material::SkyBoxMaterial*			skyBoxMaterial;
			};
			void Render(Device::DirectX& dx, const Param&& param);

			SET_ACCESSOR_DIRTY(Gamma,		float,						_gamma);
			GET_CONST_ACCESSOR(Gamma,		float,						_gamma);
			GET_ALL_ACCESSOR_PTR(ResultMap,	Texture::RenderTexture,		_resultMap);
			GET_CONST_ACCESSOR_REF(GBuffers,							_gbuffer);
			GET_CONST_ACCESSOR_REF(TBRParamCB,							_tbrCB);
			GET_ALL_ACCESSOR_REF(GlobalIllumination,					_gi);

		private:
			Texture::RenderTexture						_resultMap;
			Texture::RenderTexture						_scaledMap;
			Texture::RenderTexture						_transparentMap;
			Texture::RenderTexture						_skyBoxMap;

			GBuffers									_gbuffer;

			TBRParamCB									_tbrCB;

			Shader::ComputeShader						_tbdrShader;
			Light::OnlyLightCulling						_blendedDepthLightCulling;

			SkyBoxRenderer								_skyBoxRenderer;
			MainSceneMaker								_mainSceneMaker;

		private:
			GlobalIllumination							_gi;
			bool										_useGI = false;

		private:
			TempRenderQueue								_renderQ;
			RenderQueue::TransparentMeshRenderQueue		_transparentMeshRenderQ;

		private:
			Precompute::PreIntegrateEnvBRDF				_envBRDFMap;

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