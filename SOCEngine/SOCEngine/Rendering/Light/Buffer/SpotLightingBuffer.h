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
			class SpotLightingBuffer
			{
			private:
				typedef Buffer::GPUUploadBuffer<address, LightForm::LightTransformBuffer>	TransformBuffer;
				typedef Buffer::GPUUploadBuffer<address, Light::SpotLight::Param>		ParamBuffer;

				TransformBuffer*				_transformBuffer;
				ParamBuffer*					_paramBuffer;
				CommonLightingBuffer				_commonBuffer;
				
			public:
				SpotLightingBuffer()
					: _transformBuffer(nullptr), _paramBuffer(nullptr),
					_commonBuffer()
				{
				}
				
				~SpotLightingBuffer()
				{
					Destroy();
					
					SAFE_DELETE(_transformBuffer);
					SAFE_DELETE(_paramBuffer);					
				}
				
			public:
				void Initialize()
				{
					const __int32 dummyData[SPOT_LIGHT_BUFFER_MAX_NUM * sizeof(LightForm::LightTransformBuffer) / 4] = {0, };
					
					_transformBuffer	= TransformBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM,	DXGI_FORMAT_R32G32B32A32_FLOAT,		dummyData);
					_paramBuffer		= ParamBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM, 	DXGI_FORMAT_R16G16B16A16_FLOAT,		dummyData);

					_commonBuffer.Initialize(SPOT_LIGHT_BUFFER_MAX_NUM);
				}
				
				void Destroy()
				{
					_transformBuffer->Destroy();
					_paramBuffer->Destroy();
					
					_commonBuffer.Destroy();
				}
				
			private:
				bool UpdateBuffer(const LightWithPrevUpdateCounter& lightWithPrevUC,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
				{
					auto _UpdateBuffer = [&](const Light::LightForm* light) -> void
					{
						address key = reinterpret_cat<address>(light);
						
						LightForm::LightTransformBuffer	transform;
						SpotLight::Param		param;
						static_cast<const Light::SpotLight*>(light)->MakeParam(transform, param);
												
						// 하나만 검색해도 됨
						LightForm::LightTransformBuffer* existTarnsform = _transformBuffer->Find(key);
						if( existTarnsform == nullptr )
						{
							_transformBuffer->Add(key, transform);
							_paramBuffer->Add(key, param);
						}
						else // existTarnsform != nullptr
						{
							(*existTarnsform)		= transform;
							(*_paramBuffer->Find(key))	= param;
						}
						
						_commonBuffer.UpdateBuffer(light, getShadowIndex, getLightShaftIndex, existTarnsform != nullptr);
					};
					
					return lightWithPrevUC->UpdateBuffer(_UpdateBuffer);
				}
				
			public:
				void UpdateSRBuffer(ID3D11DeviceContext* context,
									const LightWithPrevUpdateCounter& lightWithPrevUC,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
								   bool forcedUpdate = false)
				{
					bool isNeedToUpdate = UpdateBuffer(lightWithPrevUC, getShadowIndex, getLightShaftIndex);

					if((isNeedToUpdate || forcedUpdate) == false)
						return;
					
					_transformBuffer->UpdateSRBuffer(context);
					_paramBuffer->UpdateSRBuffer(context);
					
					_commonBuffer.UpdateSRBuffer(context);
				}
				
				void Delete(const Light::SpotLight* light)
				{
					address key = reinterpret_cast<address>(light);
					
					_transformBuffer->Delete(key);
					_paramBuffer->Delete(key);
					
					_commonBuffer.Delete(key);
				}
			};
		}
	}
}
