#pragma once

#include "FullScreen.h"
#include "DepthMap.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class BilateralFiltering final
		{
		public:
			struct ParamCBData
			{
				float sigma		= 2.5f;
				float sigmaz	= 0.005f;
				float scale		= 1.0f;
				float dummy		= 0.0f;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& size, DXGI_FORMAT format);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT,
						const Texture::DepthMap& depthMap, const Texture::RenderTexture& inputColorMap,
						Texture::RenderTexture& tempMap) const;
			void UpdateParamCB(Device::DirectX& dx, const ParamCBData& data);

		private:
			FullScreen									_vertical;
			FullScreen									_horizontal;
			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
		};
	}
}