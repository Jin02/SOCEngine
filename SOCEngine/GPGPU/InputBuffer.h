#pragma once

#include "BaseShader.h"
#include "BaseBuffer.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class InputBuffer : public Rendering::Buffer::BaseBuffer
		{
		public:
			InputBuffer();
			~InputBuffer();

		public:
			bool Create(unsigned int size, unsigned int stride, const void* sysMem = nullptr);
		};
	}
}
