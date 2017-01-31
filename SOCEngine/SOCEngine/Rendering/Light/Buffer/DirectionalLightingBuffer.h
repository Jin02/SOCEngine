#pragma once

#include "LightingBufferForm.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class DirectionalLightingBuffer : public LightingBufferForm<DirectionalLight::DirXYHalf>
			{
			private:
				typedef LightingBufferForm<DirectionalLight::DirXYHalf> Parent;
			
			public:
				DirectionalLightingBuffer() : Parent()
				{
				}
				
				~DirectionalLightingBuffer()
				{
				}
				
			public:
				inline void Initialize()	{ Parent::Initialize(DIRECTIONAL_LIGHT_BUFFER_MAX_NUM); }				
				inline void Destroy()		{ Parent::Destroy(); }
				
			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount)
				{
					const __int32 dummy[maxLightCount * sizeof(DirectionalLight::DirXYHalf) / 4] = {0, };
					return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R16G16_FLOAT, dummy);				
				}
				
				virtual bool UpdateBuffer(const LightWithPrevUpdateCounter& lightWithPrevUC,
							const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
							const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
				{
					auto _UpdateBuffer = [&](const LightForm* light) -> void
					{
						address key = reinterpret_cat<address>(light);
						
						DirectionalLight::DirXYHalf dirXY;
						static_cast<const DirectionalLight*>(light)->MakeParam(dirXY);
						
						Parent::UpdateTransformBuffer(key, dirXY);
					};
					
					return lightWithPrevUC->UpdateBuffer(_UpdateBuffer);					
				}
							
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) { }			
			}
		}
	}
}
