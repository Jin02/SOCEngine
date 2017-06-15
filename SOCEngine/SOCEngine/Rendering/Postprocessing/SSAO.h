#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "Vector2.h"
#include "MainRenderer.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class SSAO final
		{
		public:
			struct ParamCBData
			{
				Math::Vector2	stepUV		= Math::Vector2(1.0f, 1.0f);
				ushort			sampleCount	= 24;
				Half			scale		= Half(10.0f);
				Half			skipDist	= Half(0.1f);
				Half			occluedRate	= Half(0.95f);
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void UpdateParam(Device::DirectX& dx, const ParamCBData& param);

			void Render(Device::DirectX& dx, Texture::RenderTexture& outRT, Texture::RenderTexture& inColorMap, Renderer::MainRenderer& mainRenderer);

		private:
			FullScreen									_screen;
			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
		};
	}
}