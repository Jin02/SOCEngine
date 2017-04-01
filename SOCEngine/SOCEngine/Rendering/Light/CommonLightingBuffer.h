#pragma once

#include "BaseLight.h"
#include "GPUUploadBuffer.hpp"
#include "ObjectId.hpp"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class CommonLightingBuffer
			{
			public:
				using KeyType					= Core::ObjectId::LiteralType;
				using ColorBuffer				= Buffer::GPUUploadBuffer<KeyType, uint>;
				using OptionalParamIndexBuffer	= Buffer::GPUUploadBuffer<KeyType, uint>;
			
				ColorBuffer						_colorBuffer;
				OptionalParamIndexBuffer		_optionalParamIndexBuffer;
			
			public:
				CommonLightingBuffer() = default;
			
				void Initialize(Device::DirectX& dx, uint count);
				void Destroy();
				
				void UpdateBuffer(const Light::BaseLight& light,
									const std::function<uchar(const Light::BaseLight&)>& getShadowIndex,
									const std::function<uchar(const Light::BaseLight&)>& getLightShaftIndex,
						 			bool existElem);
				void UpdateSRBuffer(Device::DirectX& dx);		
				void Delete(KeyType key);
				void DeleteAll();

				GET_ACCESSOR(ColorSRBuffer, const Buffer::ShaderResourceBuffer&, _colorBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, const Buffer::ShaderResourceBuffer&, _optionalParamIndexBuffer.GetShaderResourceBuffer());
			};
		}
	}
}