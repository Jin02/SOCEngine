#pragma once

#include "LightingBufferForm.hpp"
#include "SpotLight.h"
#include <assert.h>

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class SpotLightingBuffer final : public LightingBufferForm<SpotLight>
			{
			public:
				SpotLightingBuffer() = default;
				void Initialize(Device::DirectX& dx)
				{
					Parent::Initialize(dx, 2048, DXGI_FORMAT_R32G32B32A32_FLOAT);

					SpotLight::Param dummy[2048];
					_paramSRBuffer.Initialize(dx, 2048, DXGI_FORMAT_R16G16B16A16_FLOAT, dummy);
				}

				void AddLight(const SpotLight& light, const Core::Transform& lightTransform, const RequiredIndexBook& indexBooks);

				void UpdateParamBuffer(const std::vector<SpotLight*>& dirtyLights, const Core::TransformPool& transformPool);
				void UpdateSRBuffer(Device::DirectX& dx);

				void Delete(const SpotLight& light);
				void DeleteAll();

				GET_ACCESSOR(ParamSRBuffer, auto&, _paramSRBuffer.GetShaderResourceBuffer());

			private:
				using Parent = LightingBufferForm<SpotLight>;
				using Parent::Initialize;
				using Parent::AddLight;
				using Parent::UpdateSRBuffer;
				using Parent::Delete;
				using Parent::DeleteAll;

			private:		
				Buffer::GPUUploadBuffer<SpotLight::Param> _paramSRBuffer;
				bool _mustUpdateParamSRBuffer = true;
			};
		}
	}
}