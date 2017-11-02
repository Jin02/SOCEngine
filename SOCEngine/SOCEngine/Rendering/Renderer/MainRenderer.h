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
#include "LightBufferMerger.h"
#include "ComputeShader.h"

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

			GET_ALL_ACCESSOR(GBuffers,				auto&,	_gbuffer);
			GET_ALL_ACCESSOR(TBRParamCB,			auto&,	_tbrCB);

			GET_ALL_ACCESSOR(ResultMap,				auto&,	_resultMap);
			GET_ALL_ACCESSOR(DiffuseLightBuffer,	auto&,	_diffuseLightBuffer);
			GET_ALL_ACCESSOR(SpecularLightBuffer,	auto&,	_specularLightBuffer);

		private:
			LightBufferMerger			_merger;

			Texture::RenderTexture		_resultMap;

			Texture::RenderTexture		_diffuseLightBuffer;
			Texture::RenderTexture		_specularLightBuffer;
			Shader::ComputeShader		_tbdrShader;

			Light::OnlyLightCulling		_blendedDepthLC;

			TBRParamCB					_tbrCB;
			TBRCBData					_tbrCBData;

			GBuffers					_gbuffer;
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