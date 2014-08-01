#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer : public BaseBuffer
		{
		public:
			VertexBuffer();
			~VertexBuffer();

		public:
			bool Create(const void* sysMem, unsigned int byteWidth, bool isDynamic);
			void UpdateBuffer();
		};
	}
}