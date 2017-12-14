#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "DepthMap.h"

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
				float fousuNear	= 16.0f;
				float focusFar	= 18.0f;
				float blurNear	= 12.0f;
				float blurFar	= 22.0f;
			};
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outRT, 
						const Texture::RenderTexture& inColorMap,
						const MainRenderingSystemParam&& mains, const Copy& copy, TempTextures& tempTextures);

			void UpdateParamCB(Device::DirectX& dx);

			SET_ACCESSOR_DIRTY(Param, const ParamCBData&, _paramData);
			GET_CONST_ACCESSOR(Param, const ParamCBData&, _paramData);

		private:
			FullScreen									_screen;
			GaussianBlur								_blur;

			Texture::RenderTexture						_blurredColorMap;
			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
			ParamCBData									_paramData;
			bool										_dirty = true;
		};
	}
}