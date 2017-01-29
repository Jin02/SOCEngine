#pragma once

#include "GPUUploadBuffer.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class LightWithPrevUpdateCounter
			{
			private:
				const Light::LightForm*	_light;
				uint					_prevTransformUpdateCounter;
				
			public:
				LightWithPrevUpdateCounter(const Light::LightForm* light, uint prevTransformUpdateCounter)
					: _light(light), _prevTransformUpdateCounter(prevTransformUpdateCounter) {}
				~LightWithPrevUpdateCounter() {}
				
			public:
				bool UpdateBuffer(const std::function<void(const Light::LightForm* light)>& bufferUpdateCallback)
				{
					uint curCounter = _light->GetOwner()->GetTransform()->GetUpdateCounter();

					bool isChanged = curCounter != _prevTransformUpdateCounter;
					if(isChanged)
					{
						bufferUpdateCallback(_light);
						_prevTransformUpdateCounter = curCounter
					}
					
					return isChanged;
				}
			};
				

			class SpotLightingBuffer
			{
			private:
				typedef Buffer::GPUUploadBuffer<address, LightForm::LightTransformBuffer>	TransformBuffer;
				typedef Buffer::GPUUploadBuffer<address, uint>								ColorBuffer;
				typedef Buffer::GPUUploadBuffer<address, uint>								OptionalParamIndexBuffer;
				typedef Buffer::GPUUploadBuffer<address, Light::SpotLight::Param>			ParamBuffer;

				TransformBuffer*				_transformBuffer;
				ColorBuffer*					_colorBuffer;
				OptionalParamIndexBuffer*		_optionalParamIndexBuffer;
				ParamBuffer*					_paramBuffer;
				
			public:
				SpotLightingBuffer()
					: _transformBuffer(nullptr), _colorBuffer(nullptr), _optionalParamIndexBuffer(nullptr), _paramBuffer(nullptr)
				{
				}
				
				~SpotLightingBuffer()
				{
					SAFE_DELETE(_transformBuffer);
					SAFE_DELETE(_colorBuffer);
					SAFE_DELETE(_optionalParamIndexBuffer);
					SAFE_DELETE(_paramBuffer);					
				}
				
			public:
				void Initialize()
				{
					const __int32 dummyData[SPOT_LIGHT_BUFFER_MAX_NUM * sizeof(LightForm::LightTransformBuffer) / 4] = {0, };
					
					_transformBuffer			= TransformBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM,			DXGI_FORMAT_R32G32B32A32_FLOAT,		dummyData);
					_colorBuffer				= ColorBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM,				DXGI_FORMAT_R8G8B8A8_UNORM,			dummyData);
					_optionalParamIndexBuffer	= OptionalParamIndexBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM,	DXGI_FORMAT_R32_UINT,				dummyData);
					_paramBuffer				= ParamBuffer::Create(SPOT_LIGHT_BUFFER_MAX_NUM, 				DXGI_FORMAT_R16G16B16A16_FLOAT,		dummyData);
				}
				
				void Destroy()
				{
					_transformBuffer->Destroy();
					_colorBuffer->Destroy();
					_optionalParamIndexBuffer->Destroy();
					_paramBuffer->Destroy();
				}
				
			private:
				bool UpdateBuffer(const LightWithPrevUpdateCounter<Light::SpotLight>& lightWithPrevUC,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
				{
					auto _UpdateBuffer = [&](const SpotLight* light) -> void
					{
						address key = reinterpret_cat<address>(light);
						
						LightForm::LightTransformBuffer	transform;
						SpotLight::Param				param;
						light->MakeParam(transform, param);
						
						// Compute Optional Param Index
						ushort	shadowIndex				= getShadowIndex(light);
						uchar	lightShaftIndex			= getLightShaftIndex(light) & 0x7f;
						uchar	lightFlag				= light->GetFlag();
						uint	optionalParamIndex		= (shadowIndex << 16) | (lightFlag << 8) | lightShaftIndex;
						
						uint	uintColor				= light->Get32BitMainColor();
						
						// 하나만 검색해도 됨
						LightForm::LightTransformBuffer* existTarnsform = _transformBuffer->Find(key);
						if( existTarnsform == nullptr )
						{
							_transformBuffer->Add(key, transform);
							_paramBuffer->Add(key, param);
							_colorBuffer->Add(key, uintColor);
							_optionalParamIndexBuffer->Add(key, optionalParamIndex);
						}
						else // existTarnsform != nullptr
						{
							(*existTarnsform)							= transform;
							(*_paramBuffer->Find(key))					= param;
							(*_colorBuffer->Find(key))					= uintColor;
							(*_optionalParamIndexBuffer->Find(key))		= optionalParamIndex;
						}						
					};
					
					return lightWithPrevUC->UpdateBuffer(_UpdateBuffer);
				}
				
				void UpdateSRBuffer()
				{
					
				}
				
			};
		}
	}
}
