#pragma once

#include "GPUUploadBuffer.h"
#include "LightWithPrevUpdateCounter.h"
#include "CommonLightingBuffer.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			template<typename LightType>
			class LightingBufferForm
			{
			protected:
				typedef typename LightType::TransformType		TransformType;
				typedef Buffer::GPUUploadBuffer<address, TransformType>	TransformBuffer;

			private:
				TransformBuffer*								_transformBuffer;
				CommonLightingBuffer								_commonBuffer;
				
			public:
				LightingBufferForm() : _transformBuffer(nullptr), _commonBuffer()
				{
				}
				
				virtual ~LightingBufferForm()
				{
					Destroy();	
					SAFE_DELETE(_transformBuffer);
				}
				
			protected:
				void Initialize(uint maxLightCount)
				{
					_transformBuffer = InitializeTransformBuffer(maxLightCount);
					_commonBuffer.Initialize(maxLightCount);
				}

				void Destroy()
				{
					_commonBuffer.Destroy();
					_transformBuffer->Destroy();
				}

			public:
				void UpdateBuffer(	ID3D11DeviceContext* context,
							const LightWithPrevUpdateCounter& lightWithPrevUC,
							const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
							const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
							bool forcedUpdate = false	)
				{					
					auto _UpdateBuffer = [&](const LightForm* light) -> void
					{
						address key = reinterpret_cat<address>(light);
						
						TransformType transform;
						static_cast<const LightType*>(light)->MakeTransform(transform);
						
						TransformType* existTarnsform = _transformBuffer->Find(key);
						if( existTarnsform == nullptr )	_transformBuffer->Add(key, transform);
						else				(*existTarnsform) = transform;
				
						_commonBuffer.UpdateBuffer(light, getShadowIndex, getLightShaftIndex, existTarnsform != nullptr);\
						UpdateAdditionalBuffer(light);
					};
					
					bool isNeedToUpdate = lightWithPrevUC->UpdateBuffer(_UpdateBuffer);
					
					if((isNeedToUpdate || forcedUpdate) == false)
						return;
					
					_transformBuffer->UpdateSRBuffer(context);
					_commonBuffer.UpdateSRBuffer(context);
					
					UpdateAdditionalSRBuffer(context);

				}

				void Delete(const Light::PointLightingBuffer* light)
				{
					address key = reinterpret_cast<address>(light);
					
					_transformBuffer->Delete(key);
					_commonBuffer.Delete(key);
				}
				
				void DeleteAll()
				{
					_transformBuffer->DeleteAll();
					_commonBuffer.DeleteAll();					
				}
				
			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount) = 0;
				virtual void UpdateAdditionalBuffer(const LightForm* light) {}
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) {}
			};
		}
	}
}
