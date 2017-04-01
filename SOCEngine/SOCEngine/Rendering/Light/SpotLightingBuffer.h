#pragma once

#include "LightingBufferForm.hpp"
#include "SpotLight.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class SpotLightingBuffer final
			{
			public:
				SpotLightingBuffer() = default;
				void Initialize(Device::DirectX& dx);

				void UpdateBuffer(std::vector<SpotLight>& lights,
					const std::function<uchar(const BaseLight&)>& getShadowIndex,
					const std::function<uchar(const BaseLight&)>& getLightShaftIndex,
					const Core::TransformPool& pool);

				void UpdateSRBuffer(Device::DirectX& dx);
				void Delete(Core::ObjectId id);
				void DeleteAll();

			private:
				LightingBufferForm<SpotLight>	_base;

				Buffer::GPUUploadBuffer<Core::ObjectId::LiteralType, SpotLight::Param> _paramSRBuffer;
				bool _mustUpdateParamSRBuffer = true;
			};
		}
	}
}