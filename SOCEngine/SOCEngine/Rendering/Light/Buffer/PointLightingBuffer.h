#pragma once

#include "LightingBufferForm.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class PointLightingBuffer : public LightingBufferForm<LightForm::LightTransformBuffer>
			{
			private:
				typedef LightingBufferForm<LightForm::LightTransformBuffer> Parent;

			public:
				PointLightingBuffer();
				~PointLightingBuffer();
				
			public:
				inline void Initialize()	{ Parent::Initialize(POINT_LIGHT_BUFFER_MAX_NUM); }				
				inline void Destroy()		{ Parent::Destroy(); }
				
			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount);
				virtual bool UpdateBuffer(const LightWithPrevUpdateCounter& lightWithPrevUC,
							const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
							const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) { }
			};
		}
	}
}
