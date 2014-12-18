#pragma once

#include "BaseBuffer.h"
#include <functional>

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer;

		class CPUReadConstBuffer : private BaseBuffer
		{
		public:
			CPUReadConstBuffer();
			~CPUReadConstBuffer();

		public:
			bool Create(unsigned int size, unsigned int stride);
			void Read(ID3D11DeviceContext* context, BaseBuffer* target, const std::function<void(const void* dataRecive)>& dataReceiveFunc);

		public:
			GET_ACCESSOR(Buffer, ID3D11Buffer*, _buffer);
		};

	}
}