#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "DepthBuffer.h"

#include "MainRenderer.h"
#include "MainCamera.h"

#include "Copy.h"
#include "GaussianBlur.h"

#include "PPCommon.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class Bloom final
		{
		public:
			struct ParamCBData
			{
				float dt					= 0.0f;
				float exposureStrength		= 0.1f;
				float exposureSpeed			= 0.4f;
				float bloomThreshold		= 0.001f;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void UpdateParam(Device::DirectX& dx, const ParamCBData& param);

			void RenderThresholdMap(
				Device::DirectX& dx, Texture::RenderTexture& inColorMap, Copy& copy,
				TempTextures& tempTextures, Renderer::MainRenderer& renderer);

			void RenderBloom(Device::DirectX& dx, Texture::RenderTexture& outRT, Texture::RenderTexture& inputColorMap, Renderer::MainRenderer& mainRenderer);

		private:
			FullScreen									_eyeAdaptation;
			FullScreen									_bloomThreshold;
			FullScreen									_bloom;

			GaussianBlur								_blur;

			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;

			std::array<Texture::RenderTexture, 2>		_adaptedLuminanceMaps; //prev, curr
			Texture::RenderTexture						_bloomThresholdMap;

			bool										_currentAdaptedLuminanceIndx = false;
		};
	}
}