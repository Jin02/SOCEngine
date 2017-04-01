#pragma once

#include "LightingBufferForm.hpp"
#include "DirectionalLight.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class DirectionalLightingBuffer : public LightingBufferForm<DirectionalLight>
			{
			public:
				using LightingBufferForm<DirectionalLight>::LightingBufferForm;
				void Initialize(Device::DirectX& dx) { Initialize(dx, 1024, DXGI_FORMAT_R16G16_FLOAT); }

			private:
				using LightingBufferForm<DirectionalLight>::Initialize;
			};
		}
	}
}