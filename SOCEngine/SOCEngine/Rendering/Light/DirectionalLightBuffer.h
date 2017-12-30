#pragma once

#include "LightBufferForm.hpp"
#include "DirectionalLight.h"

namespace Rendering
{
	namespace Light
	{
		namespace Buffer
		{
			class DirectionalLightBuffer : public LightBufferForm<DirectionalLight>
			{
			public:
				using Parent = LightBufferForm<DirectionalLight>;
				using Parent::LightBufferForm;
				void Initialize(Device::DirectX& dx)
				{
					Initialize(dx, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R16G16_FLOAT);
				}

			private:
				using Parent::Initialize;
			};
		}
	}
}