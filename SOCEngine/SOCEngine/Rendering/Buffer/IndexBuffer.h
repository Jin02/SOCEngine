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
			bool Initialize(const unsigned int* sysMem, unsigned int byteWidth, bool isDynamic = false);
			void IASetBuffer(ID3D11DeviceContext* context);
		};
	}
}