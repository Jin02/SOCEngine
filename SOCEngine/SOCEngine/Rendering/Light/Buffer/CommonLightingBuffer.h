#pragma once

#include "LightForm.h"
#include "GPUUploadBuffer.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class CommonLightingBuffer
			{
			private:
				typedef Buffer::GPUUploadBuffer<address, uint>		ColorBuffer;
				typedef Buffer::GPUUploadBuffer<address, uint>		OptionalParamIndexBuffer;			
			
				ColorBuffer*										_colorBuffer;
				OptionalParamIndexBuffer*							_optionalParamIndexBuffer;
			
			public:
				CommonLightingBuffer()
					: _colorBuffer(nullptr), _optionalParamIndexBuffer(nullptr)
				{
				}
				
				~CommonLightingBuffer()
				{
					SAFE_DELETE(_colorBuffer);
					SAFE_DELETE(_optionalParamIndexBuffer);
				}
			
			protected:
				void Initialize(uint count)
				{
					_colorBuffer				= ColorBuffer::Create(count,				DXGI_FORMAT_R8G8B8A8_UNORM,			dummyData);
					_optionalParamIndexBuffer	= OptionalParamIndexBuffer::Create(count,	DXGI_FORMAT_R32_UINT,				dummyData);	
				}
				
				void Destroy()
				{
					_colorBuffer->Destroy();
					_optionalParamIndexBuffer->Destroy();
				}
				
				void UpdateBuffer(const Light::LightForm* light,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex)
				{
						address key = reinterpret_cat<address>(light);
						
						// Compute Optional Param Index
						ushort	shadowIndex				= getShadowIndex(light);
						uchar	lightShaftIndex			= getLightShaftIndex(light) & 0x7f;
						uchar	lightFlag				= light->GetFlag();
						uint	optionalParamIndex		= (shadowIndex << 16) | (lightFlag << 8) | lightShaftIndex;
						
						uint	uintColor				= light->Get32BitMainColor();
						
						uint* existColor = _colorBuffer->Find(key);
						if(existColor == nullptr)
						{
							_colorBuffer->Add(key, uintColor);
							_optionalParamIndexBuffer->Add(key, optionalParamIndex);
						}
						else	// existColor != nullptr
						{
							(*existColor)							= uintColor;
							(*_optionalParamIndexBuffer->Find(key))	= optionalParamIndex;
						}
				}
				
				void UpdateSRBuffer(ID3D11DeviceContext* context)
				{
					_colorBuffer->UpdateSRBuffer(context);
					_optionalParamIndexBuffer->UpdateSRBuffer(context);
				}
				
				void Delete(const Light::LightForm* light)
				{
					address key = reinterpret_cast<address>(light);
					
					_colorBuffer->Delete(key);
					_optionalParamIndexBuffer->Delete(key);				
				}
			};
		}
	}
}
