#pragma once

#include "DirectX.h"

namespace Rendering
{
	namespace Buffer
	{
		class BaseBuffer
		{
		protected:
			ID3D11Buffer*	_buffer;

		public:
			BaseBuffer();
			virtual ~BaseBuffer();

		public:
			void UpdateSubResource(ID3D11DeviceContext* context, const void* data);
			void UpdateResourceUsingMapUnMap(ID3D11DeviceContext* context, const void* data, uint size);
			void UpdateResourceUsingMapUnMap(ID3D11DeviceContext* context, const void* data, uint startOffset, uint size, D3D11_MAP mapType);

		public:
			GET_ACCESSOR(Buffer, ID3D11Buffer*, _buffer);
		};
	}
}