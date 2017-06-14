#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"
#include "ShaderManager.h"
#include "ConstBuffer.h"
#include "ComputeShader.h"
#include "LightManager.h"

namespace Rendering
{
	namespace Camera
	{
		class MainCamera;
	}

	namespace Light
	{
		class OnlyLightCulling final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderRectSize);
			void Dispatch(Device::DirectX& dx, Camera::MainCamera& mainCamera, Manager::LightManager& lightMgr);

			GET_ACCESSOR(LightIndexSRBuffer,	auto&,	_srb);
			GET_ACCESSOR(LightIndexUAV,			auto&,	_uav);

		private:
			Rendering::Buffer::ShaderResourceBuffer	_srb;
			View::UnorderedAccessView				_uav;
			Shader::ComputeShader					_cs;
		};
	}
}