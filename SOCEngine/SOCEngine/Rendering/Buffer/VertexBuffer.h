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
			bool Initialize(const void* sysMem, unsigned int bufferSize, unsigned int count, bool isDynamic);
			void IASetBuffer(ID3D11DeviceContext* context);
			void UpdateVertexData(ID3D11DeviceContext* context, const void* data, uint size);
		};
	}
}