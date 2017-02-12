#pragma once

#include "LightForm.h"
#include "GPUUploadBuffer.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class CommonLightingBuffer
			{
			private:
				typedef Rendering::Buffer::GPUUploadBuffer<address, uint>		ColorBuffer;
				typedef Rendering::Buffer::GPUUploadBuffer<address, uint>		OptionalParamIndexBuffer;			
			
				ColorBuffer*						_colorBuffer;
				OptionalParamIndexBuffer*				_optionalParamIndexBuffer;
			
			public:
				CommonLightingBuffer();	
				~CommonLightingBuffer();
			
			public:
				void Initialize(uint count);				
				void Destroy();
				
				void UpdateBuffer(const Light::LightForm* light,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex,
						 			bool existElem);
				void UpdateSRBuffer(ID3D11DeviceContext* context);		
				void Delete(const Light::LightForm* light);
				void DeleteAll();
				
				GET_ACCESSOR(ColorSRBuffer,			const Buffer::ShaderResourceBuffer*,	_colorBuffer->GetShaderResourceBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer,	const Buffer::ShaderResourceBuffer*,	_optionalParamIndexBuffer->GetShaderResourceBuffer());
				
			};
		}
	}
}