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
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount)
				{
					const __int32 dummy[maxLightCount * sizeof(LightForm::LightTransformBuffer) / 4] = {0, };
					return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R32G32B32A32_FLOAT, dummy);
				}
				
				virtual bool UpdateBuffer(const LightWithPrevUpdateCounter& lightWithPrevUC,
							const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
							const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
				{
					auto _UpdateBuffer = [&](const LightForm* light) -> void
					{
						address key = reinterpret_cat<address>(light);
						
						LightForm::LightTransformBuffer	transform;
						static_cast<const PointLight*>(light)->MakeParam(transform);
						
						Parent::UpdateTransformBuffer(key, transform);
					};
					
					return lightWithPrevUC->UpdateBuffer(_UpdateBuffer);
				}
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) { }
			};
		}
	}
}
