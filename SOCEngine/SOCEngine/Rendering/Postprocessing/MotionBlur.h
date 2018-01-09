#pragma once

#include "PPCommon.h"

#include "FullScreen.h"
#include "ConstBuffer.h"
#include "MainRenderer.h"

#include "GaussianBlur.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class MotionBlur final
		{
		public:
			struct ParamCBData
			{
				float	length		= 0.025f;
				float	kernelSize	= 7.5f;
				float	kernelStep	= 1.0f;
				uint	dummy;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Destroy();

			void UpdateParamCB(Device::DirectX& dx);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outRT, const Texture::RenderTexture& inColorMap, const Renderer::MainRenderer& mainRenderer);

			SET_ACCESSOR_DIRTY(Param, const ParamCBData&, _paramData);
			GET_CONST_ACCESSOR(Param, const ParamCBData&, _paramData);

		private:
			FullScreen									_screen;

			Buffer::ExplicitConstBuffer<ParamCBData>	_paramCB;
			ParamCBData									_paramData;
			bool										_dirty = true;
		};
	}
}