#pragma once

#include "GaussianBlur.h"
#include "DepthOfField.h"
#include "SSAO.h"
#include "Bloom.h"
#include "MainCamera.h"
#include "SunShaft.h"

namespace Rendering
{
	namespace Manager
	{
		class PostProcessPipeline final
		{
		public:
			PostProcessPipeline() = default;
			DISALLOW_ASSIGN_COPY(PostProcessPipeline);

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera, bool useBloom);
			void Render(Device::DirectX& dx, Renderer::MainRenderer& mainRenderer, const Camera::MainCamera& mainCamera, const Manager::LightManager& lightMgr);

			void UpdateCB(Device::DirectX& dx, const Core::ObjectManager& objMgr, const Manager::LightManager& lightMgr, const Core::TransformPool& tfPool, const Camera::MainCamera& mainCam);

			void ReCompileBloom(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera, bool use);

			SET_ACCESSOR(UseSSAO,			bool,	_useSSAO);
			SET_ACCESSOR(UseDoF,			bool,	_useDoF);
			SET_ACCESSOR(UseSunShaft,		bool,	_useSunShaft)

			GET_CONST_ACCESSOR(UseSSAO,		bool,	_useSSAO);
			GET_CONST_ACCESSOR(UseDoF,		bool,	_useDoF);
			GET_CONST_ACCESSOR(UseSunShaft, bool,	_useSunShaft);
			GET_CONST_ACCESSOR(UseBloom,	bool,	GetPostproessing<PostProcessing::Bloom>().GetUse());

			inline void SetElapsedTime(float time)
			{
				GetPostproessing<PostProcessing::Bloom>().SetElapsedTime(time);
			}
			inline void SetSunShaftParam(Core::ObjectID directionalLightID, float circleSize, float circleIntensity)
			{
				GetPostproessing<PostProcessing::SunShaft>().SetCircleIntensity(Half(circleIntensity));
				GetPostproessing<PostProcessing::SunShaft>().SetCircleSize(Half(circleSize));
				GetPostproessing<PostProcessing::SunShaft>().SetDirectionalLightID(directionalLightID);
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

			std::tuple<	PostProcessing::Bloom,
						PostProcessing::SSAO,
						PostProcessing::DepthOfField,
						PostProcessing::SunShaft>		_postprocessing;

			PostProcessing::Copy						_copy;

			bool										_useSSAO		= false;
			bool										_useDoF			= false;
			bool										_useSunShaft	= false;
		};
	}
}