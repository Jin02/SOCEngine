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
			template<typename TransformType>
			class LightingBufferForm
			{
			protected:
				typedef Buffer::GPUUploadBuffer<address, TransformType>	TransformBuffer;

			private:
				TransformBuffer*					_transformBuffer;
				CommonLightingBuffer					_commonBuffer;
				
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
				
				void UpdateTransformBuffer(uint key, const TransformType& transform)
				{
					LightForm::LightTransformBuffer* existTarnsform = _transformBuffer->Find(key);
					if( existTarnsform == nullptr )
						_transformBuffer->Add(key, transform);
					else
						(*existTarnsform) = transform;				
				}

			public:
				void UpdateSRBuffer(ID3D11DeviceContext* context,
								const LightWithPrevUpdateCounter& lightWithPrevUC,
								const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
								const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
								bool forcedUpdate = false)
				{
					bool isNeedToUpdate = UpdateBuffer(lightWithPrevUC, getShadowIndex, getLightShaftIndex);
					_commonBuffer.UpdateBuffer(light, getShadowIndex, getLightShaftIndex, isNeedToUpdate);

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
				virtual bool UpdateBuffer(const LightWithPrevUpdateCounter& lightWithPrevUC,
							const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
							const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex) = 0;
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) = 0;
			};
		}
	}
}
