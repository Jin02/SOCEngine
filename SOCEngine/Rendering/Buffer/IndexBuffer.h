#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class IndexBuffer : public BaseBuffer
		{		
		public:
			IndexBuffer();
			~IndexBuffer();

		public:
			bool Create(const ENGINE_INDEX_TYPE* sysMem, unsigned int byteWidth);
			void UpdateBuffer();
		};
	}
}