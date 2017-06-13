#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"
#include "ShaderManager.h"
#include "ConstBuffer.h"
#include "MainCamera.h"
#include "ComputeShader.h"
#include "LightManager.h"

namespace Rendering
{
	namespace Light
	{
		class OnlyLightCulling final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Camera::MainCamera& mainCamera);

			void Dispatch(Device::DirectX& dx, Camera::MainCamera& mainCamera, Manager::LightManager& lightMgr);

			GET_ACCESSOR(LightIndexSRBuffer,	auto&, _srb);
			GET_ACCESSOR(LightIndexUAV,			auto&,	_uav);

		private:
			Buffer::ShaderResourceBuffer	_srb;
			View::UnorderedAccessView		_uav;
			Shader::ComputeShader			_cs;
		};
	}
}