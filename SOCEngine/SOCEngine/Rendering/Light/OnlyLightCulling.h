#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"
#include "ShaderManager.h"
#include "ConstBuffer.h"
#include "ComputeShader.h"
#include "LightManager.h"
#include "MainCamera.h"
#include "TileBasedShadingHeader.h"

namespace Rendering
{
	namespace Light
	{
		class OnlyLightCulling final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& maxRenderRectSize);
			void Dispatch(Device::DirectX& dx, const Shader::ComputeShader::ThreadGroup& group);
			void Dispatch(Device::DirectX& dx, const Shader::ComputeShader::ThreadGroup&& group) { Dispatch(dx, group); }

			GET_ALL_ACCESSOR(LightIndexSRBuffer,	auto&,	_srb);
			GET_ALL_ACCESSOR(LightIndexUAV,			auto&,	_uav);

		private:
			Rendering::Buffer::ShaderResourceBuffer	_srb;
			View::UnorderedAccessView				_uav;
			Shader::ComputeShader					_cs;
		};
	}
}