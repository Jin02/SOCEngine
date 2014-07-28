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
			bool Create(unsigned int bindFlags, D3D11_USAGE usage, const void* sysMem, unsigned int byteWidth);

		public:
			GET_ACCESSOR(Buffer, const ID3D11Buffer*, _buffer);
		};
	}
}