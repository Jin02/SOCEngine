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
				CommonLightingBuffer();	
				~CommonLightingBuffer();
			
			public:
				void Initialize(uint count);				
				void Destroy();
				
				void UpdateBuffer(const Light::LightForm* light,
									const std::function<uchar(const Light::LightForm*)>& getShadowIndex,
									const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);				
				void UpdateSRBuffer(ID3D11DeviceContext* context);				
				void Delete(const Light::LightForm* light);
			};
		}
	}
}
