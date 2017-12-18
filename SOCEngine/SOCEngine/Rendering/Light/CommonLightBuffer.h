#pragma once

#include "BaseLight.h"
#include "GPUUploadBuffer.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace Buffer
		{
			class CommonLightBuffer
			{
			public:
				using ColorBuffer				= Rendering::Buffer::GPUUploadBuffer<uint>;
				using OptionalParamIndexBuffer	= Rendering::Buffer::GPUUploadBuffer<uint>;
			
			public:
				CommonLightBuffer() = default;
			
				void Initialize(Device::DirectX& dx, uint count, const void* dummy);
				
				void SetData(uint index, const Light::BaseLight& light, ushort shadowIndex, uint lightShaftIndex);
				void PushData(const Light::BaseLight& light, ushort shadowIndex, uint lightShaftIndex);

				void UpdateSRBuffer(Device::DirectX& dx);
				void Delete(uint index);
				void DeleteAll();

				GET_CONST_ACCESSOR_REF(ColorSRBuffer,				_colorBuffer.GetShaderResourceBuffer());
				GET_CONST_ACCESSOR_REF(OptionalParamIndexSRBuffer,	_optionalParamIndexBuffer.GetShaderResourceBuffer());

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
