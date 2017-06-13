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
				void Initialize(Device::DirectX& dx);

				void AddLight(const SpotLight& light);

				void UpdateTransformBuffer(	const std::vector<SpotLight*>& dirtyTFLights,
											const Core::TransformPool& transformPool );
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
				using Parent::UpdateTransformBuffer;

			private:		
				Buffer::GPUUploadBuffer<SpotLight::Param> _paramSRBuffer;
			};
		}
	}
}