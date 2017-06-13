#pragma once

#include "LightingBufferForm.hpp"
#include "PointLight.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class PointLightingBuffer : public LightingBufferForm<PointLight>
			{
			public:
				using LightingBufferForm<PointLight>::LightingBufferForm;
				void Initialize(Device::DirectX& dx) { Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT); }

			private:
				using LightingBufferForm<PointLight>::Initialize;
			};
		}
	}
}