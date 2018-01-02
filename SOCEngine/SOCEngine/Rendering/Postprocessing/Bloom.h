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
				float exposureStrength		= 0.2f;
				float exposureSpeed			= 0.8f;
				float bloomThreshold		= 0.4f;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize, bool use);
			void Destroy();

			void UpdateParamCB(Device::DirectX& dx);

			void RenderThresholdMap(Device::DirectX& dx, const Texture::RenderTexture& inColorMap, const Copy& copy, TempTextures& tempTextures, const Renderer::TBRParamCB& tbrParamCB);
			void RenderBloom(Device::DirectX& dx, Texture::RenderTexture& outRT, const Texture::RenderTexture& inputColorMap, const Renderer::TBRParamCB& tbrParamCB);

			SET_ACCESSOR(ElapsedTime,	float,				_paramData.dt);
			SET_ACCESSOR(Param,			const ParamCBData&,	_paramData);
			GET_CONST_ACCESSOR(Param,	const ParamCBData&,	_paramData);

			GET_CONST_ACCESSOR(Use, bool, _use);

		private:
			FullScreen									_eyeAdaptation;
			FullScreen									_bloomThreshold;
			FullScreen									_bloom;

			GaussianBlur								_blur;

			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;

			std::array<Texture::RenderTexture, 2>		_adaptedLuminanceMaps; //prev, curr
			Texture::RenderTexture						_bloomThresholdMap;

			bool										_currentAdaptedLuminanceIndx	= false;
			bool										_use							= false;
			
			ParamCBData									_paramData;
		};
	}
}