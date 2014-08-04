#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer : public BaseBuffer
		{	
		public:
			ConstBuffer();
			~ConstBuffer();

		public:
			bool Create(unsigned int size)
			{
				return BaseBuffer::Create(D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT, nullptr, size);
			}

			void UpdateSubresource(const void* data)
			{
				ID3D11DeviceContext* context;
				context->UpdateSubresource(_buffer, 0, nullptr, data, 0, 0);
			}
		};
	}
}