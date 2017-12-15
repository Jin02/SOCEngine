#pragma once

#include "GaussianBlur.h"
#include "DepthOfField.h"
#include "SSAO.h"
#include "Bloom.h"
#include "MainCamera.h"

namespace Rendering
{
	namespace Manager
	{
		class PostProcessPipeline
		{
		public:
			PostProcessPipeline() = default;

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera);
			const Texture::RenderTexture* Render(Device::DirectX& dx, Renderer::MainRenderer& mainRenderer, const Camera::MainCamera& mainCamera);

			void UpdateCB(Device::DirectX& dx);

			SET_ACCESSOR(UseSSAO,		bool,	_useSSAO);
			SET_ACCESSOR(UseDoF,		bool,	_useDoF);
			GET_CONST_ACCESSOR(UseSSAO, bool,	_useSSAO);
			GET_CONST_ACCESSOR(UseDoF,	bool,	_useDoF);

			inline void SetElapsedTime(float time)
			{
				GetPostproessing<PostProcessing::Bloom>().SetElapsedTime(time);
			}

		public:
			template <class Postprocessing> const auto& GetParam() const
			{
				return GetPostproessing<Postprocessing>().GetParam();
			}
			template <class Postprocessing> void SetParam(const typename Postprocessing::ParamCBData& param)
			{
				GetPostproessing<Postprocessing>().SetParam(param);
			}
		private:
			template <class Postprocessing> auto& GetPostproessing()
			{
				return std::get<Postprocessing>(_postprocessing);
			}
			template <class Postprocessing> const auto& GetPostproessing() const
			{
				return std::get<Postprocessing>(_postprocessing);
			}

		public:
			PostProcessing::TempTextures				_tempTextures;

			Texture::RenderTexture						_tempResultMap;
			Texture::RenderTexture						_bluredCurScene;

			std::tuple<	PostProcessing::Bloom,
						PostProcessing::SSAO,
						PostProcessing::DepthOfField>	_postprocessing;

			PostProcessing::Copy						_copy;

			bool										_useSSAO = false;
			bool										_useDoF = false;
		};
	}
}