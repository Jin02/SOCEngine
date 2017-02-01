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
				TransformBuffer*					_transformBuffer;
				CommonLightingBuffer					_commonBuffer;
				bool							_isNeedToUpdateSRBuffer;
				
			public:
				LightingBufferForm() : _transformBuffer(nullptr), _commonBuffer(), _isNeedToUpdateSRBuffer(false)
				{
				}
				
				virtual ~LightingBufferForm()
				{
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
				void UpdateBuffer(const std::vector<LightWithPrevUpdateCounter<LightType>>& lightWithPrevUCs,
						  const std::function<uchar(const LightType*)>& getShadowIndex,
						  const std::function<uchar(const LightType*)>& getLightShaftIndex)
				{					
					auto _UpdateBuffer = [&](const LightType* light) -> void
					{
						address key = reinterpret_cat<address>(light);
						
						TransformType transform;
						light->MakeTransform(transform);
						
						TransformType* existTarnsform = _transformBuffer->Find(key);
						if( existTarnsform == nullptr )	_transformBuffer->Add(key, transform);
						else				(*existTarnsform) = transform;
				
						_commonBuffer.UpdateBuffer(light, getShadowIndex, getLightShaftIndex, existTarnsform != nullptr);\
						UpdateAdditionalBuffer(light);
					};
					
					_isNeedToUpdateSRBuffer = false;
					for(const auto& iter : lightWithPrevUCs)
						_isNeedToUpdateSRBuffer |= iter.UpdateBuffer(_UpdateBuffer);					
				}
				
				void UpdateSRBuffer(ID3D11DeviceContext* context, bool forcedUpdate = false)
				{
					if(_isNeedToUpdateSRBuffer || forcedUpdate)
					{
						_transformBuffer->UpdateSRBuffer(context);
						_commonBuffer.UpdateSRBuffer(context);
					
						UpdateAdditionalSRBuffer(context);
					}
					
					_isNeedToUpdateSRBuffer = false;
				}
				
				void Delete(const LightType* light)
				{
					address key = reinterpret_cast<address>(light);
					
					_transformBuffer->Delete(key);
					_commonBuffer.Delete(key);
					
					OnDelete(light);
				}
				
				void DeleteAll()
				{
					_transformBuffer->DeleteAll();
					_commonBuffer.DeleteAll();
					
					OnDeleteAll();
				}
				
			private:
				virtual TransformBuffer* InitializeTransformBuffer(uint maxLightCount) = 0;
				virtual void UpdateAdditionalBuffer(const LightType* light, bool existElem) {}
				virtual void UpdateAdditionalSRBuffer(ID3D11DeviceContext* context) {}
				virtual void OnDelete(const LightType* light) { }
				virtual void OnDeleteAll() {}
			};
		}
	}
}
