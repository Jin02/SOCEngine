#pragma once

#include "FullScreen.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class GaussianBlur final
		{
		public:
			struct ParamCBData
			{
				float sigma				= 2.5f;
				float numPixelPerSide	= 6.0f;
				float blurSize			= 8.0f;
				float scale				= 1.0f;

				ParamCBData() = default;
				ParamCBData(float _sigma, float _numPixelPerSide, float _blurSize, float _scale)
					: sigma(_sigma), numPixelPerSide(_numPixelPerSide), blurSize(_blurSize), scale(_scale)
				{ }
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);

			void UpdateParamCB(Device::DirectX& dx, const ParamCBData& param);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT,  const Texture::RenderTexture& inputColorMap, Texture::RenderTexture& tempMap) const;

		private:
			FullScreen									_vertical;
			FullScreen									_horizontal;

			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
		};
	}
}