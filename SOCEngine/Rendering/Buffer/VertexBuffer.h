#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer : public BaseBuffer
		{
		private:
			unsigned int _stride;

		public:
			VertexBuffer();
			~VertexBuffer();

		public:
			bool Create(const void* sysMem, unsigned int bufferSize, unsigned int count, bool isDynamic);
			void UpdateBuffer();
		};
	}
}