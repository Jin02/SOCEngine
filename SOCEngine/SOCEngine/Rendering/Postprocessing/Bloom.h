#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "DepthMap.h"

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
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize);
			void UpdateParamCB(Device::DirectX& dx);

			void RenderThresholdMap(Device::DirectX& dx, const Texture::RenderTexture& inColorMap, const Copy& copy, TempTextures& tempTextures, const Renderer::MainRenderer& renderer);
			void RenderBloom(Device::DirectX& dx, Texture::RenderTexture& outRT, const Texture::RenderTexture& inputColorMap, const Renderer::MainRenderer& mainRenderer);

			SET_ACCESSOR(ElapsedTime,	float,				_paramData.dt);
			SET_ACCESSOR(Param,			const ParamCBData&,	_paramData);
			GET_CONST_ACCESSOR(Param,	const ParamCBData&,	_paramData);

		private:
			FullScreen									_eyeAdaptation;
			FullScreen									_bloomThreshold;
			FullScreen									_bloom;

			GaussianBlur								_blur;

			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;

			std::array<Texture::RenderTexture, 2>		_adaptedLuminanceMaps; //prev, curr
			Texture::RenderTexture						_bloomThresholdMap;

			bool										_currentAdaptedLuminanceIndx = false;
			
			ParamCBData									_paramData;
		};
	}
}