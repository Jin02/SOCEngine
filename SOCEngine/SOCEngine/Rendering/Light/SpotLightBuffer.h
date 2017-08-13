#pragma once

#include "LightBufferForm.hpp"
#include "SpotLight.h"
#include <assert.h>

namespace Rendering
{
	namespace Light
	{
		namespace Buffer
		{
			class SpotLightBuffer final : public LightBufferForm<SpotLight>
			{
			public:
				SpotLightBuffer() = default;
				void Initialize(Device::DirectX& dx);

				void PushLight(const SpotLight& light);

				void UpdateTransformBuffer(	const std::vector<SpotLight*>& dirtyTFLights,
											const Core::TransformPool& transformPool,
											const LightDatasIndexer& indexer );
				void UpdateSRBuffer(Device::DirectX& dx);

				void Delete(uint index);
				void DeleteAll();

				GET_ACCESSOR(ParamSRBuffer, auto&, _paramSRBuffer.GetShaderResourceBuffer());

			private:
				using Parent = LightBufferForm<SpotLight>;
				using Parent::Initialize;
				using Parent::PushLight;
				using Parent::UpdateSRBuffer;
				using Parent::Delete;
				using Parent::DeleteAll;
				using Parent::UpdateTransformBuffer;

			private:		
				Rendering::Buffer::GPUUploadBuffer<SpotLight::Param> _paramSRBuffer;
			};
		}
	}
}