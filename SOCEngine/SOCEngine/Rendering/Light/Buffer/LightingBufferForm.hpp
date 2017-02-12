#pragma once

#include "GPUUploadBuffer.hpp"
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
				typedef typename LightType::TransformType				TransformType;
				typedef Buffer::GPUUploadBuffer<address, TransformType>	TransformBuffer;

			private:
				TransformBuffer*					_transformBuffer;
				CommonLightingBuffer				_commonBuffer;
				
			public:
				LightingBufferForm() : _transformBuffer(nullptr), _commonBuffer()
				{
				}
				
				~LightingBufferForm()
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
				void UpdateBuffer(const std::vector<LightType>& lights,
						  const std::function<uchar(const LightType*)>& getShadowIndex,
						  const std::function<uchar(const LightType*)>& getLightShaftIndex)
				{					
					for(const auto& iter : lights)
					{
						address key = 0;//GetKey(light);
						assert(key > 0);
						
						TransformType transform;
						iter->MakeTransform(transform);
						
						TransformType* existTarnsform = _transformBuffer->Find(key);
						if( existTarnsform == nullptr )	_transformBuffer->Add(key, transform);
						else							(*existTarnsform) = transform;
				
						_commonBuffer.UpdateBuffer(iter, getShadowIndex, getLightShaftIndex, existTarnsform != nullptr);
					}
				}
				
				void UpdateSRBuffer(ID3D11DeviceContext* context)
				{
					_transformBuffer->UpdateSRBuffer(context);
					_commonBuffer.UpdateSRBuffer(context);
				}
				
				void Delete(const LightType* light)
				{
					address key = 0;//GetKey(light);
					assert(key > 0);
					
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
				
			public:
				GET_ACCESSOR(TransformSRBuffer,				const ShaderResourceBuffer*, _transformBuffer->GetShaderResourceBuffer());
				GET_ACCESSOR(ColorSRBuffer,					const ShaderResourceBuffer*, _commonBuffer.GetColorSRBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, 	const ShaderResourceBuffer*, _commonBuffer.GetOptionalParamIndexSRBuffer());
			};
		}
	}
}