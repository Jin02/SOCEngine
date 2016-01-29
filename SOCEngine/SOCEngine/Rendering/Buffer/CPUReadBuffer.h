#pragma once

#include "BaseBuffer.h"
#include <functional>

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer;

		class CPUReadBuffer : private BaseBuffer
		{
		public:
			CPUReadBuffer();
			virtual ~CPUReadBuffer();

		public:
			bool Initialize(unsigned int stride, unsigned int num);
			void Read(ID3D11DeviceContext* context, ID3D11Resource* target, const std::function<void(const void* dataRecive)>& dataReceiveFunc);

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}

		public:
			GET_ACCESSOR(Buffer, ID3D11Buffer*, _buffer);
		};

	}
}