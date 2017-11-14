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