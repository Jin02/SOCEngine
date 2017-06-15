#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "DepthBuffer.h"

#include "MainRenderer.h"
#include "MainCamera.h"

#include "PPCommon.h"
#include "Copy.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class DepthOfField final
		{
		public:
			struct ParamCBData
			{
				float fousuNear	= 10.0f;
				float blurNear	= 9.0f;
				float focusFar	= 15.0f;
				float blurFar	= 16.0f;
			};
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void UpdateParam(Device::DirectX& dx, const ParamCBData& param);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outRT, 
				Texture::RenderTexture& inColorMap,
				Main& mains, Copy& copy, TempTextures& tempTextures);

		private:
			FullScreen									_screen;
			GaussianBlur								_blur;

			Texture::RenderTexture						_blurredColorMap;
			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
		};
	}
}