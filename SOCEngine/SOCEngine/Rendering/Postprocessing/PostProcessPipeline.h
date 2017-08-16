#pragma once

#include "GaussianBlur.h"
#include "DepthOfField.h"
#include "SSAO.h"
#include "Bloom.h"

namespace Rendering
{
	namespace Manager
	{
		class PostProcessPipeline
		{
		public:
			PostProcessPipeline() = default;

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Render(Device::DirectX& dx, Renderer::MainRenderer& mainRenderer, Camera::MainCamera& mainMeshCamera);

			void UpdateCB(Device::DirectX& dx);

			GET_SET_ACCESSOR(UseSSAO, bool, _useSSAO);
			GET_SET_ACCESSOR(UseDoF, bool, _useDoF);
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

		private:
			PostProcessing::TempTextures			_tempTextures;

			Texture::RenderTexture					_tempResultMap;
			Texture::RenderTexture					_bluredCurScene;

			std::tuple<	PostProcessing::Bloom,
						PostProcessing::SSAO,
						PostProcessing::DepthOfField> _postprocessing;


			PostProcessing::Copy					_copy;

			bool									_useSSAO = false;
			bool									_useDoF = false;
		};
	}
}