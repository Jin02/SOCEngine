#pragma once

#include "LightingBufferForm.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class DirectionalLightingBuffer : public LightingBufferForm<DirectionalLight>
			{
			private:
				typedef LightingBufferForm<DirectionalLight> Parent;

			public:
				DirectionalLightingBuffer();
				~DirectionalLightingBuffer();
				
			public:
				inline void Initialize()	{ Parent::Initialize(DIRECTIONAL_LIGHT_BUFFER_MAX_NUM); }				
				inline void Destroy()		{ Parent::Destroy(); }
				static DirectionalLightingBuffer* Create();
				
			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount);
			};
		}
	}
}
