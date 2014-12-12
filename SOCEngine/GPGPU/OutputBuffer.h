#pragma once

#include "BaseShader.h"
#include "BaseBuffer.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class OutputBuffer : public Rendering::Buffer::BaseBuffer
		{
		public:
			OutputBuffer();
			~OutputBuffer();

		public:
			bool Create(unsigned int size, unsigned int stride);
		};
	}
}
