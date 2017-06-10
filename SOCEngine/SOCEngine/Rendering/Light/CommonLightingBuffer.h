#pragma once

#include "BaseLight.h"
#include "GPUUploadBuffer.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class CommonLightingBuffer
			{
			public:
				using ColorBuffer				= Buffer::GPUUploadBuffer<uint>;
				using OptionalParamIndexBuffer	= Buffer::GPUUploadBuffer<uint>;
			
			public:
				CommonLightingBuffer() = default;
			
				void Initialize(Device::DirectX& dx, uint count, const void* dummy);
				
				void SetBufferData(const Light::BaseLight& light, ushort shadowIndex, uint lightShaftIndex);
				void AddBufferData(const Light::BaseLight& light, ushort shadowIndex, uint lightShaftIndex);

				void UpdateSRBuffer(Device::DirectX& dx);		
				void Delete(LightId id);
				void DeleteAll();

				GET_ACCESSOR(ColorSRBuffer, const Buffer::ShaderResourceBuffer&, _colorBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, const Buffer::ShaderResourceBuffer&, _optionalParamIndexBuffer.GetShaderResourceBuffer());

			private:
				inline uint ComputeOptionalParamIndex(const Light::BaseLight& light, ushort shadowIndex, uint shaftIndex)
				{
					return (shadowIndex << 16) | (light.GetFlag() << 8) | (shaftIndex & 0x7f);
				}

			private:
				ColorBuffer						_colorBuffer;
				OptionalParamIndexBuffer		_optionalParamIndexBuffer;
			};
		}
	}
}
