#pragma once

#include "LightingBufferForm.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class PointLightingBuffer : public LightingBufferForm<PointLight>
			{
			private:
				typedef LightingBufferForm<PointLight> Parent;

			public:
				PointLightingBuffer();
				~PointLightingBuffer();
				
			public:
				inline void Initialize()	{ Parent::Initialize(POINT_LIGHT_BUFFER_MAX_NUM); }				
				inline void Destroy()		{ Parent::Destroy(); }
				
				static PointLightingBuffer* Create();
				
			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount);
			};
		}
	}
}