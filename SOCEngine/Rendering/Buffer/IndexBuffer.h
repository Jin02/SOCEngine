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
			bool Create(const WORD* sysMem, unsigned int byteWidth);
			void UpdateBuffer();
		};
	}
}