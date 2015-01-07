#pragma once

#include "BaseBuffer.h"
#include <functional>

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer;

		class ConstBuffer_CPURead : private BaseBuffer
		{
		public:
			ConstBuffer_CPURead();
			~ConstBuffer_CPURead();

		public:
			bool Create(unsigned int stride, unsigned int num);
			void Read(ID3D11DeviceContext* context, BaseBuffer* target, const std::function<void(const void* dataRecive)>& dataReceiveFunc);

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}

		public:
			GET_ACCESSOR(Buffer, ID3D11Buffer*, _buffer);
		};

	}
}