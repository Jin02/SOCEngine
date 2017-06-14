#pragma once

#include "LightBufferForm.hpp"
#include "PointLight.h"

namespace Rendering
{
	namespace Light
	{
		namespace Buffer
		{
			class PointLightBuffer : public LightBufferForm<PointLight>
			{
			public:
				using LightBufferForm<PointLight>::LightBufferForm;
				void Initialize(Device::DirectX& dx) { Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT); }

			private:
				using LightBufferForm<PointLight>::Initialize;
			};
		}
	}
}