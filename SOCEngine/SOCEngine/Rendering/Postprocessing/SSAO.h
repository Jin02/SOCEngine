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
			void UpdateParamCB(Device::DirectX& dx);

			void Render(Device::DirectX& dx, Texture::RenderTexture& outRT, const Texture::RenderTexture& inColorMap, const Renderer::MainRenderer& mainRenderer) const;

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